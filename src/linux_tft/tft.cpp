#pragma GCC optimize("O3")
#include "tft.h"

void set_addr_window(int16_t x0, int16_t y0, uint16_t w, uint16_t h)
{
  DC_HIGH();
  write_window_addr(x0, y0, w, h);
}

void push_img_buff(const uint8_t* img_buff, size_t len)
{
  CS_LOW();

  set_addr_window(0, 0, TFT_WIDTH, TFT_HEIGHT);

  DC_HIGH();

  spi_transfer_message(img_buff, len);

  CS_HIGH();
}

bool tft_init()
{
  if (!spi_init(TFT_WIDTH * TFT_HEIGHT * sizeof(uint16_t)))
    return false;

  if (!add_pin_init())
    return false;

  init_display();

  set_rotation(DISPLAY_ROTATION);
  invert_display(INVERT_COLORS);

  return true;
}

void tft_deinit()
{
  add_pin_set(PIN_BLK, LOW);
  spi_deinit();
  add_pin_deinit();
}

uint16_t rgb_to_bgr(uint16_t rgb)
{
  uint8_t r = (rgb >> 11) & 0x1F;
  uint8_t g = (rgb >> 5) & 0x3F;
  uint8_t b = rgb & 0x1F;
  return (b << 11) | (g << 5) | r;
}