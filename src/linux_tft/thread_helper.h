#pragma once

#include <thread>
#include <chrono>

#define delay(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
