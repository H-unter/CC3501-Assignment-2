#pragma once
#include <stdint.h>
#include "hardware/pio.h"

extern pio_program_t ws2812_program;

void ws2812_program_init(PIO pio, unsigned int sm, unsigned int offset, unsigned int pin, float freq, bool rgbw);
