#pragma once 

#include <stdint.h>
#include <chrono>

typedef std::chrono::time_point<std::chrono::steady_clock,std::chrono::steady_clock::duration> absolute_time_t;

uint32_t to_ms_since_boot(absolute_time_t t);
absolute_time_t get_absolute_time();
