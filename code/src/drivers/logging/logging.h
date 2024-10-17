#pragma once
#include "ff.h" // Include FatFs to define FRESULT
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "drivers/sd_card/sd_card.h"
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

// data logging functions

struct data_sample
{
    uint64_t elapsed_time_ms;
    float loadcell_voltage;
    float loadcell_weight;
    float dac_voltage;

    data_sample() : elapsed_time_ms(0), loadcell_voltage(0.0f), loadcell_weight(0.0f), dac_voltage(0.0f) {}
};

void write_csv_header(SDCard &sd_card, FIL &file);
void write_csv_row(SDCard &sd_card, FIL &file, struct data_sample &data);
