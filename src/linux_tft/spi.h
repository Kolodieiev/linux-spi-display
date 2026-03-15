#pragma once
#pragma GCC optimize("O3")

#include <cstdint>
#include "thread_helper.h"

typedef enum
{
  BEGIN_WRITE,
  WRITE_COMMAND_8,
  WRITE_COMMAND_16,
  WRITE_COMMAND_BYTES,
  WRITE_DATA_8,
  WRITE_DATA_16,
  WRITE_BYTES,
  WRITE_C8_D8,
  WRITE_C8_D16,
  WRITE_C8_BYTES,
  WRITE_C16_D16,
  END_WRITE,
  DELAY,
} spi_operation_type_t;

bool spi_init(uint32_t max_buf_size);
void spi_deinit();
void spi_transfer_message(const uint8_t* tx_buf, size_t len);

void batch_operations(const uint8_t* operations, size_t len);

void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2);
void writeC8D8(uint8_t c, uint8_t d);
void writeCommand(uint8_t c);
void beginWrite();
void endWrite();