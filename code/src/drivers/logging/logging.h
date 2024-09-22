#pragma once
#include "ff.h" // Include FatFs to define FRESULT
/// Represents the priority of a log message.
enum LogLevel
{
    INFORMATION,
    WARNING,
    ERROR,
};

/// Set the log level. Messages with a level below this threshold will be discarded.
void setLogLevel(LogLevel newLevel);

/// Log a new message.
void log(LogLevel level, const char *msg);

// Convert an FRESULT to a string.
const char *FRESULT_str(FRESULT fr);