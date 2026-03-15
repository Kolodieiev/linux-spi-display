#include "linux_tft/tft.h"

int main()
{
  tft_init();

  size_t len = TFT_WIDTH * TFT_HEIGHT * sizeof(uint16_t);

  uint16_t* buff = (uint16_t*)malloc(len);

  for (size_t i = 0; i < len / 4; ++i)
    buff[i] = bswap_16(rgb_to_bgr(COLOR_RED)); // ST7796
    // buff[i] = bswap_16(COLOR_RED); // ST7789

  for (size_t i = len / 4; i < len / 2; ++i)
    buff[i] = bswap_16(rgb_to_bgr(COLOR_BLACK));
    // buff[i] = bswap_16(COLOR_BLACK);

  push_img_buff((uint8_t*)buff, len);

  free(buff);

  tft_deinit();

  return 0;
}