#pragma once
#include <cstdint>

#define DISPLAY_ROTATION 0

// #define ST7735
// #define ST7789
#define ST7796

#ifdef ST7796

#define INVERT_COLORS false
#define TFT_WIDTH 320
#define TFT_HEIGHT 480

#elifdef ST7789

#define INVERT_COLORS true
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

#endif  // #ifdef ST7796

enum AdditionalPin : uint8_t
{
  PIN_RST = 69,
  PIN_BLK = 70,
  PIN_DC = 72,
  PIN_CS = 74,
};

static const char* STR_GPIO_PATH = "/dev/gpiochip0";
static const char* STR_SPIDEV_PATH = "/dev/spidev1.1";

#define SPI_FREQUENCY 50000000