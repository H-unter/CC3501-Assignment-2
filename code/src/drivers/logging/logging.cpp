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
    if (level < maxLogLevel)
    {
        return;
    }

    // Get the time since boot
    uint32_t time = to_ms_since_boot(get_absolute_time());
    uint32_t time_sec = time / 1000;
    uint32_t time_decimal = (time % 1000);

    // Convert the level to a string
    const char *levelStr;
    switch (level)
    {
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

const char *FRESULT_str(FRESULT fr)
{
    switch (fr)
    {
    case FR_OK:
        return "Success";
    case FR_DISK_ERR:
        return "Disk Error";
    case FR_INT_ERR:
        return "Internal Error";
    case FR_NOT_READY:
        return "Not Ready";
    case FR_NO_FILE:
        return "No File";
    case FR_NO_PATH:
        return "No Path";
    case FR_INVALID_NAME:
        return "Invalid Name";
    case FR_DENIED:
        return "Access Denied";
    case FR_EXIST:
        return "File Already Exists";
    case FR_INVALID_OBJECT:
        return "Invalid Object";
    case FR_WRITE_PROTECTED:
        return "Write Protected";
    case FR_INVALID_DRIVE:
        return "Invalid Drive";
    case FR_NOT_ENABLED:
        return "Not Enabled";
    case FR_NO_FILESYSTEM:
        return "No Filesystem";
    case FR_MKFS_ABORTED:
        return "MKFS Aborted";
    case FR_TIMEOUT:
        return "Timeout";
    case FR_LOCKED:
        return "Locked";
    case FR_NOT_ENOUGH_CORE:
        return "Not Enough Core";
    case FR_TOO_MANY_OPEN_FILES:
        return "Too Many Open Files";
    case FR_INVALID_PARAMETER:
        return "Invalid Parameter";
    default:
        return "Unknown Error";
    }
}
