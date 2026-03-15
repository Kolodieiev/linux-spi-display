#pragma GCC optimize("O3")
#include "spi.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "add_pins.h"
#include "setup.h"

#define SPI_CHUNK_SIZE 256
#define SPI_TR_PACK_SIZE 10

static const uint32_t SPI_FREQ = SPI_FREQUENCY;
static const uint32_t SPI_MODE = SPI_MODE_0;
static const uint32_t SPI_BIT_PER_WORD = 8;

static struct spi_ioc_transfer* _transfers = nullptr;
static size_t _max_chunks_num = 0;
static int spi_fd = -1;

static union
{
  uint16_t value;
  struct
  {
    uint8_t lsb;
    uint8_t msb;
  };
} _data16;

static union
{
  uint8_t* _buffer{nullptr};
  uint16_t* _buffer16;
  uint32_t* _buffer32;
};

static uint16_t _data_buf_index = 0;

void POLL(uint32_t len);
void flush_data_buf();
void WRITE8BIT(uint8_t d);

#define MSB_16(val) (((val) & 0xFF00) >> 8) | (((val) & 0xFF) << 8)
#define MSB_16_SET(var, val) \
  {                          \
    (var) = MSB_16(val);     \
  }
#define MSB_32_SET(var, val)                                  \
  {                                                           \
    uint8_t* v = (uint8_t*)&(val);                            \
    (var) = v[3] | (v[2] << 8) | (v[1] << 16) | (v[0] << 24); \
  }
#define MSB_32_16_16_SET(var, v1, v2)                                                                                   \
  {                                                                                                                     \
    (var) = (((uint32_t)v2 & 0xff00) << 8) | (((uint32_t)v2 & 0xff) << 24) | ((v1 & 0xff00) >> 8) | ((v1 & 0xff) << 8); \
  }
#define MSB_32_8_ARRAY_SET(var, a)                                  \
  {                                                                 \
    (var) = ((uint32_t)a[0] << 8 | a[1] | a[2] << 24 | a[3] << 16); \
  }

bool spi_init(uint32_t max_buf_size)
{
  if (spi_fd > -1)
    return true;

  spi_fd = open(STR_SPIDEV_PATH, O_RDWR);
  if (spi_fd < 0)
  {
    fprintf(stderr, "Помилка відкриття spidev\n");
    return false;
  }

  if (ioctl(spi_fd, SPI_IOC_WR_MODE, &SPI_MODE) < 0)
  {
    fprintf(stderr, "Помилка встановлення SPI MODE\n");
    return false;
  }

  if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &SPI_BIT_PER_WORD) < 0)
  {
    fprintf(stderr, "Помилка встановлення BITS_PER_WORD\n");
    return false;
  }

  if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_FREQ) < 0)
  {
    fprintf(stderr, "Помилка встановлення MAX_SPEED_HZ\n");
    return false;
  }

  // Загальна кількість чанків
  _max_chunks_num = (max_buf_size + SPI_CHUNK_SIZE - 1) / SPI_CHUNK_SIZE;

  // Виділяємо масив для всіх чанків
  _transfers = (struct spi_ioc_transfer*)calloc(_max_chunks_num, sizeof(struct spi_ioc_transfer));

  if (!_transfers)
  {
    fprintf(stderr, "Помилка calloc для SPI transfers\n");
    return false;
  }

  _buffer = (uint8_t*)malloc(SPI_CHUNK_SIZE);
  if (!_buffer)
  {
    fprintf(stderr, "Помилка malloc для SPI _buffer\n");
    free(_transfers);
    return false;
  }

  // Встановлюємо загальні параметри один раз, щоб не робити цього в циклі відправки
  // ВАЖЛИВО: CS має змінюватися між пакетами ioctl, але залишатися
  // активним (0) всередині кожного ioctl, тому ми не встановлюємо transfers[i].cs_change.
  for (size_t i = 0; i < _max_chunks_num; ++i)
  {
    _transfers[i].speed_hz = SPI_FREQ;
    _transfers[i].bits_per_word = SPI_BIT_PER_WORD;
    // Інші поля 0 завдяки calloc.
  }

  printf("Шину SPI ініціалізовано\n");
  return true;
}

void spi_deinit()
{
  if (spi_fd > 0)
    close(spi_fd);
  free(_transfers);
  _transfers = nullptr;
  free(_buffer);
  _buffer = nullptr;

  _max_chunks_num = 0;

  printf("Шину SPI деініціалізовано\n");
}

void spi_transfer_message(const uint8_t* tx_buf, size_t len)
{
  if (len == 0)
    return;

  size_t total_chunks = (len + SPI_CHUNK_SIZE - 1) / SPI_CHUNK_SIZE;

  if (total_chunks > _max_chunks_num)
  {
    fprintf(stderr, "Завеликий розмір даних для буфера SPI transfers\n");
    return;
  }

  // Формуємо всі транзакції в масиві
  for (size_t i = 0; i < total_chunks; i++)
  {
    size_t offset = i * SPI_CHUNK_SIZE;
    size_t chunk_len = (offset + SPI_CHUNK_SIZE <= len) ? SPI_CHUNK_SIZE : (len - offset);

    _transfers[i].tx_buf = (unsigned long)(tx_buf + offset);
    _transfers[i].len = chunk_len;
  }

  // Оптимізація для уникнення обмежень ядра
  size_t total_sent_chunks = 0;

  while (total_sent_chunks < total_chunks)
  {
    // Визначаємо, скільки чанків відправити в цьому ioctl:
    size_t chunks_to_send = (total_chunks - total_sent_chunks < SPI_TR_PACK_SIZE) ? (total_chunks - total_sent_chunks) : SPI_TR_PACK_SIZE;

    // Відправляємо пакет
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(chunks_to_send), &_transfers[total_sent_chunks]) < 0)
    {
      fprintf(stderr, "Помилка SPI_IOC_MESSAGE у пакетному циклі\n");
      printf("total_chunks: %zu\nchunks_to_send: %zu\ntotal_sent_chunks: %zu\n", total_chunks, chunks_to_send, total_sent_chunks);
      break;
    }

    total_sent_chunks += chunks_to_send;
  }
}

void write16(uint16_t d)
{
  _data16.value = d;

  WRITE8BIT(_data16.msb);
  WRITE8BIT(_data16.lsb);
}

void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  writeCommand(c);
  write16(d1);
  write16(d2);
}

void POLL(uint32_t len)
{
  spi_transfer_message(_buffer, len);
}

void beginWrite()
{
  _data_buf_index = 0;
  _buffer[0] = 0;

  DC_HIGH();
  CS_LOW();
}

void endWrite()
{
  if (_data_buf_index > 0)
    flush_data_buf();

  CS_HIGH();
}

void writeCommand(uint8_t c)
{
  if (_data_buf_index > 0)
  {
    flush_data_buf();
  }

  DC_LOW();

  _buffer[0] = c;

  POLL(sizeof(uint8_t));

  DC_HIGH();
}

void writeCommand16(uint16_t c)
{
  if (_data_buf_index > 0)
    flush_data_buf();

  DC_LOW();

  MSB_16_SET(_buffer[0], c);

  POLL(sizeof(uint16_t));

  DC_HIGH();
}

void writeCommandBytes(uint8_t* data, uint32_t len)
{
  DC_LOW();

  while (len--)
  {
    WRITE8BIT(*data++);
  }

  DC_HIGH();
}

void flush_data_buf()
{
  POLL(_data_buf_index);
  _data_buf_index = 0;
}

void WRITE8BIT(uint8_t d)
{
  _buffer[_data_buf_index] = d;
  ++_data_buf_index;
}

void writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();
  _buffer[0] = c;
  POLL(7);

  DC_HIGH();
  _buffer[0] = d;
  POLL(7);
}

void batch_operations(const uint8_t* operations, size_t len)
{
  for (size_t i = 0; i < len; ++i)
  {
    uint8_t l = 0;
    switch (operations[i])
    {
      case BEGIN_WRITE:
        beginWrite();
        break;
      case WRITE_C8_D16:
        l++;
        /* fall through */
      case WRITE_C8_D8:
        l++;
        /* fall through */
      case WRITE_COMMAND_8:
        writeCommand(operations[++i]);
        break;
      case WRITE_C16_D16:
        l = 2;
        /* fall through */
      case WRITE_COMMAND_16:
        _data16.msb = operations[++i];
        _data16.lsb = operations[++i];
        writeCommand16(_data16.value);
        break;
      case WRITE_COMMAND_BYTES:
        l = operations[++i];
        writeCommandBytes((uint8_t*)(operations + i + 1), l);
        i += l;
        l = 0;
        break;
      case WRITE_DATA_8:
        l = 1;
        break;
      case WRITE_DATA_16:
        l = 2;
        break;
      case WRITE_BYTES:
        l = operations[++i];
        break;
      case WRITE_C8_BYTES:
        writeCommand(operations[++i]);
        l = operations[++i];
        break;
      case END_WRITE:
        endWrite();
        break;
      case DELAY:
        delay(operations[++i]);
        break;
      default:
        printf("Unknown operation id at %zu: %u\n", i, operations[i]);
        break;
    }

    while (l--)
    {
      WRITE8BIT(operations[++i]);
    }
  }
}
