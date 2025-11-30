#include "tft.h"

#include <byteswap.h>

#include <cerrno>
#include <cstdio>
#include <cstring>

#include "displ_add_line_ctrl.h"
#include "spi_bus.h"
#include "thread_helper.h"

#define WIDTH 240
#define HEIGHT 320
#define ST7796_DELAY 120
#define BYTES_PER_PIXEL 2

#define ST7796_COLMOD 0x3A
#define ST7796_SLPOUT 0x11
#define ST7796_DISPON 0x29

void init_st7796();

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

#define COLOR_BLACK 0x0000
#define COLOR_NAVY 0x000F
#define COLOR_DARKGREEN 0x03E0
#define COLOR_DARKCYAN 0x03EF
#define COLOR_MAROON 0x7800
#define COLOR_PURPLE 0x780F
#define COLOR_OLIVE 0x7BE0
#define COLOR_LIGHTGREY 0xD69A
#define COLOR_LIME 0xA7E0
#define COLOR_DARKGREY 0x3186
#define COLOR_GREY 0xAD55
#define COLOR_BLUE 0x001F
#define COLOR_GREEN 0x07E0
#define COLOR_CYAN 0x07FF
#define COLOR_RED 0xF800
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW 0xFFE0
#define COLOR_WHITE 0xFFFF
#define COLOR_ORANGE 0xFDA0
#define COLOR_GREENYELLOW 0xB7E0
#define COLOR_PINK 0xFE19
#define COLOR_BROWN 0x9A60
#define COLOR_GOLD 0xFEA0
#define COLOR_SILVER 0xC618
#define COLOR_SKYBLUE 0x867D
#define COLOR_VIOLET 0x915C

uint16_t rgb_to_bgr(uint16_t rgb)
{
  uint8_t r = (rgb >> 11) & 0x1F;
  uint8_t g = (rgb >> 5) & 0x3F;
  uint8_t b = rgb & 0x1F;
  return (b << 11) | (g << 5) | r;
}

bool init_tft()
{
  if (!init_spi_bus(TFT_WIDTH * TFT_HEIGHT * sizeof(uint16_t)))
    return false;

  if (!init_disp_add_pins())
    return false;

  init_st7796();

  size_t len = TFT_WIDTH * TFT_HEIGHT * sizeof(uint16_t);

  uint16_t* buff = (uint16_t*)malloc(len);

  for (size_t i = 0; i < len / 4; ++i)
    buff[i] = bswap_16(rgb_to_bgr(COLOR_BLUE));

  for (size_t i = len / 4; i < len / 2; ++i)
    buff[i] = bswap_16(rgb_to_bgr(COLOR_YELLOW));

  push_img_buff((uint8_t*)buff, len);

  free(buff);

  return true;
}

void deinit_tft()
{
  set_disp_add_pin(PIN_BLK, LOW);
  deinit_spi_bus();
  deinit_disp_add_pins();
}

void init_st7796()
{
  set_disp_add_pin(PIN_BLK, HIGH);

  set_disp_add_pin(PIN_RST, HIGH);
  delay(100);
  set_disp_add_pin(PIN_RST, LOW);
  delay(ST7796_DELAY);
  set_disp_add_pin(PIN_RST, HIGH);
  delay(ST7796_DELAY);

  batch_operations(st7796_init_operations, sizeof(st7796_init_operations));
  // invert_display(true); // Перевертає дисплей
}
