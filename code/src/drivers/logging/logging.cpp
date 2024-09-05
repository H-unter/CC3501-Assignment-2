// Logging system, using the style that state is global in the C file.

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "logging.h"

// --- Device driver internal state:

/// Drop messages whose level is below this threshold.
static LogLevel maxLogLevel = LogLevel::INFORMATION;

// --- Device driver functions
void setLogLevel(LogLevel newLevel)
{
    maxLogLevel = newLevel;
}

void log(LogLevel level, const char *msg)
{
    // Should we show this message?
    if (level < maxLogLevel) {
        return;
    }

    // Get the time since boot
    uint32_t time = to_ms_since_boot(get_absolute_time());
    uint32_t time_sec = time / 1000;
    uint32_t time_decimal = (time % 1000);

    // Convert the level to a string
    const char *levelStr;
    switch (level) {
        case LogLevel::INFORMATION:
            levelStr = "Information";
            break;
        case LogLevel::WARNING:
            levelStr = "Warning";
            break;
        case LogLevel::ERROR:
            levelStr = "Error";
            break;
        
    };
    printf("[%u.%03u %s]: %s\n", time_sec, time_decimal, levelStr, msg);
}
