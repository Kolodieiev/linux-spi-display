#pragma once
#pragma GCC optimize("O3")
#include <byteswap.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstdint>

#include "add_pins.h"
#include "setup.h"
#include "spi.h"

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

bool tft_init();
void tft_deinit();
void set_addr_window(int16_t x0, int16_t y0, uint16_t w, uint16_t h);
void push_img_buff(const uint8_t* img_buff, size_t len);
uint16_t rgb_to_bgr(uint16_t rgb);

//

void invert_display(bool state);
void init_display();
void write_window_addr(int16_t x, int16_t y, uint16_t w, uint16_t h);
void set_rotation(uint8_t r);
