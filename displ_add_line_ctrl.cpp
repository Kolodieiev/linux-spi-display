#include "displ_add_line_ctrl.h"

#include <fcntl.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unordered_map>

const char* STR_CHIP_PATH = "/dev/gpiochip0";
const char* STR_CONS_LBL = "display_add_pin";

const std::unordered_map<DisplayPin, uint> PIN_MAP{
    {PIN_RST, 0},
    {PIN_BLK, 1},
    {PIN_DC, 2},
    {PIN_CS, 3},
};

int _chip_fd = -1;
int _line_fd = -1;

struct gpiohandle_request _request;
struct gpiohandle_data _gpio_data;

bool init_disp_add_pins()
{
  _chip_fd = open(STR_CHIP_PATH, O_RDONLY);
  if (_chip_fd < 0)
  {
    fprintf(stderr, "Помилка відкриття gpiochip\n");
    printf("Перевір:\n");
    printf("  ls -la /dev/gpiochip*\n");
    printf("  cat /sys/class/gpio/gpiochip*/label\n");
    return false;
  }

  printf("GPIO chip відкрито\n");

  memset(&_gpio_data, 0, sizeof(_gpio_data));
  memset(&_request, 0, sizeof(_request));
  _request.lineoffsets[PIN_MAP.at(PIN_BLK)] = PIN_BLK;
  _request.lineoffsets[PIN_MAP.at(PIN_RST)] = PIN_RST;
  _request.lineoffsets[PIN_MAP.at(PIN_DC)] = PIN_DC;
  _request.lineoffsets[PIN_MAP.at(PIN_CS)] = PIN_CS;

  _request.flags = GPIOHANDLE_REQUEST_OUTPUT;
  _request.lines = PIN_MAP.size();
  _request.default_values[PIN_MAP.at(PIN_BLK)] = LOW;
  _request.default_values[PIN_MAP.at(PIN_RST)] = HIGH;
  _request.default_values[PIN_MAP.at(PIN_DC)] = LOW;
  _request.default_values[PIN_MAP.at(PIN_CS)] = HIGH;

  strcpy(_request.consumer_label, STR_CONS_LBL);

  if (ioctl(_chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &_request) < 0)
  {
    fprintf(stderr, "Помилка взяття GPIO line\n");
    printf("Можливо line зайнятий або потрібні права root\n");
    close(_chip_fd);
    return false;
  }

  _line_fd = _request.fd;
  printf("GPIO line отримано (fd=%d)\n\n", _line_fd);
  return true;
}

void deinit_disp_add_pins()
{
  if (_line_fd > 0)
    close(_line_fd);

  if (_chip_fd > 0)
    close(_chip_fd);

  printf("Додаткові піни дисплея деініціалізовано\n");
}

void set_disp_add_pin(DisplayPin pin, u_char state)
{
  _gpio_data.values[PIN_MAP.at(pin)] = state;

  if (ioctl(_line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &_gpio_data) < 0)
  {
    fprintf(stderr, "Помилка встановлення встановлення піна [%u] в стан [%u]\n", pin, state);
  }
}
