#pragma once
#include <stdlib.h>
#include <unistd.h>

#include <cstdint>

#define TFT_WIDTH 320
#define TFT_HEIGHT 480

bool init_tft();
void deinit_tft();
