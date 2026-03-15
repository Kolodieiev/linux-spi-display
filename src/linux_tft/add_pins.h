#pragma once
#pragma GCC optimize("O3")
#include <cstdlib>

#include "setup.h"

#define HIGH 1
#define LOW 0

bool add_pin_init();
void add_pin_deinit();
void add_pin_set(AdditionalPin pin, u_char state);

void DC_HIGH();
void DC_LOW();
void CS_HIGH();
void CS_LOW();