#include "pico/time.h"

absolute_time_t get_absolute_time() 
{   
    return std::chrono::steady_clock::now();
}

uint32_t to_ms_since_boot(absolute_time_t t)
{
    auto duration = t.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return (uint32_t)millis;
}
