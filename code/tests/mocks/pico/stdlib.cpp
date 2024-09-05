#include <iostream>
#include <thread>
#include <chrono>

#include "pico/stdlib.h"
#include "ws2812.pio.h"

void stdio_init_all()
{

}

void sleep_ms(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void sleep_us(uint32_t us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}
