#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <cstdint>

#define HIGH 1
#define LOW 0

enum DisplayPin : uint8_t
{
  PIN_RST = 69,
  PIN_BLK = 70,
  PIN_DC = 72,
  PIN_CS = 74,
};

bool init_disp_add_pins();
void deinit_disp_add_pins();

void set_disp_add_pin(DisplayPin pin, u_char state);