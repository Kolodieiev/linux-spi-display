#pragma GCC optimize("O3")
#include "tft.h"

#ifdef ST7796

#define BYTES_PER_PIXEL 2
#define ST7796_DELAY 120

#define ST7796_NOP 0x00
#define ST7796_SWRESET 0x01
#define ST7796_RDDID 0x04
#define ST7796_RDDST 0x09

#define ST7796_SLPIN 0x10
#define ST7796_SLPOUT 0x11
#define ST7796_PTLON 0x12
#define ST7796_NORON 0x13

#define ST7796_INVOFF 0x20
#define ST7796_INVON 0x21
#define ST7796_DISPOFF 0x28
#define ST7796_DISPON 0x29

#define ST7796_CASET 0x2A
#define ST7796_RASET 0x2B
#define ST7796_RAMWR 0x2C
#define ST7796_RAMRD 0x2E

#define ST7796_PTLAR 0x30
#define ST7796_COLMOD 0x3A
#define ST7796_MADCTL 0x36

#define ST7796_MADCTL_MY 0x80
#define ST7796_MADCTL_MX 0x40
#define ST7796_MADCTL_MV 0x20
#define ST7796_MADCTL_ML 0x10
#define ST7796_MADCTL_RGB 0x00
#define ST7796_MADCTL_BGR 0x08
#define ST7796_MADCTL_MH 0x04

#define ST7796_RDID1 0xDA
#define ST7796_RDID2 0xDB
#define ST7796_RDID3 0xDC
#define ST7796_RDID4 0xDD

static const uint8_t st7796_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8,
    ST7796_COLMOD,
    0x55,  // 0x66,

    WRITE_C8_D8,
    0xF0,
    0xC3,  // Command Set Control
    WRITE_C8_D8,
    0xF0,
    0x96,

    WRITE_C8_D8,
    0xB4,
    0x01,

    WRITE_COMMAND_8,
    0xB6,
    WRITE_BYTES,
    3,
    0x80,
    0x22,
    0x3B,

    WRITE_COMMAND_8,
    0xE8,
    WRITE_BYTES,
    8,
    0x40,
    0x8A,
    0x00,
    0x00,
    0x29,
    0x19,
    0xA5,
    0x33,

    WRITE_C8_D8,
    0xC1,
    0x06,
    WRITE_C8_D8,
    0xC2,
    0xA7,
    WRITE_C8_D8,
    0xC5,
    0x18,

    WRITE_COMMAND_8,
    0xE0,
    WRITE_BYTES,
    14,
    0xF0,
    0x09,
    0x0B,
    0x06,
    0x04,
    0x15,
    0x2F,
    0x54,
    0x42,
    0x3C,
    0x17,
    0x14,
    0x18,
    0x1B,

    WRITE_COMMAND_8,
    0xE1,
    WRITE_BYTES,
    14,
    0xE0,
    0x09,
    0x0B,
    0x06,
    0x04,
    0x03,
    0x2B,
    0x43,
    0x42,
    0x3B,
    0x16,
    0x14,
    0x17,
    0x1B,

    WRITE_C8_D8,
    0xF0,
    0x3C,
    WRITE_C8_D8,
    0xF0,
    0x69,
    WRITE_COMMAND_8,
    ST7796_SLPOUT,
    END_WRITE,

    DELAY,
    ST7796_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8,
    0x38,
    WRITE_COMMAND_8,
    ST7796_DISPON,
    END_WRITE,

    DELAY,
    ST7796_DELAY};

void invert_display(bool state)
{
  beginWrite();
  writeCommand((state) ? ST7796_INVON : ST7796_INVOFF);
  endWrite();
}

void write_window_addr(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  writeC8D16D16(ST7796_CASET, x, w - 1);

  writeC8D16D16(ST7796_RASET, y, h - 1);

  writeCommand(ST7796_RAMWR);  // write to RAM
}

void init_display()
{
  add_pin_set(PIN_BLK, HIGH);
  add_pin_set(PIN_RST, HIGH);
  delay(100);
  add_pin_set(PIN_RST, LOW);
  delay(ST7796_DELAY);
  add_pin_set(PIN_RST, HIGH);
  delay(ST7796_DELAY);

  batch_operations(st7796_init_operations, sizeof(st7796_init_operations));
}

void set_rotation(uint8_t r)
{
  switch (r)
  {
    case 1:
      r = ST7796_MADCTL_MX | ST7796_MADCTL_MV | ST7796_MADCTL_BGR;
      break;
    case 2:
      r = ST7796_MADCTL_MX | ST7796_MADCTL_MY | ST7796_MADCTL_BGR;
      break;
    case 3:
      r = ST7796_MADCTL_MY | ST7796_MADCTL_MV | ST7796_MADCTL_BGR;
      break;
    case 4:
      r = ST7796_MADCTL_MX | ST7796_MADCTL_BGR;
      break;
    case 5:
      r = ST7796_MADCTL_MX | ST7796_MADCTL_MY | ST7796_MADCTL_MV | ST7796_MADCTL_BGR;
      break;
    case 6:
      r = ST7796_MADCTL_MY | ST7796_MADCTL_BGR;
      break;
    case 7:
      r = ST7796_MADCTL_MV | ST7796_MADCTL_BGR;
      break;
    default:  // case 0:
      r = ST7796_MADCTL_BGR;
      break;
  }
  beginWrite();
  writeC8D8(ST7796_MADCTL, r);
  endWrite();
}

#endif  // #ifdef ST7796