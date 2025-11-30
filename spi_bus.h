#pragma once

#include <stdlib.h>
#include <unistd.h>

#include <cstdint>

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

bool init_spi_bus(uint max_buf_size);
void deinit_spi_bus();
void spi_transfer_message(const uint8_t* tx_buf, size_t len);
void batch_operations(const uint8_t* operations, size_t len);

void push_img_buff(const uint8_t* img_buff, size_t len);

void invert_display(bool state);