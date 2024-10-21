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

/*!
 * \brief Represents a single data sample containing sensor readings and elapsed time.
 *
 * This structure stores the most recent data from the load cell and DAC, as well as
 * the elapsed time since the system started.
 *
 * \param elapsed_time_ms The time in milliseconds since the system booted
 *
 * \param loadcell_voltage The voltage reading from the load cell at the time of sampling.
 *
 * \param loadcell_weight The measured weight from the load cell at the time of sampling.
 *
 * \param dac_voltage The theoretical voltage output from the DAC at the time of sampling.
 */
struct data_sample
{
    uint64_t elapsed_time_ms;
    float loadcell_voltage;
    float loadcell_weight;
    float dac_voltage;
    float leaf_temperature;
    float sap_flow;

    data_sample() : elapsed_time_ms(0), loadcell_voltage(0.0f), loadcell_weight(0.0f), dac_voltage(0.0f), leaf_temperature(0.0f), sap_flow(0.0f) {}
};

/*!
 * \brief Writes the CSV header to the specified file on the SD card for nicely formatted tabulated data
 *
 * \param sd_card A reference to the SDCard object that handles the SD card operations.
 * \param file A reference to the FIL object representing the open CSV file on the SD card.
 *
 * \return void
 */
void write_csv_header(SDCard &sd_card, FIL &file);

/*!
 * \brief Writes a single row of data to the specified file on the SD card in CSV format.
 *
 * \param sd_card A reference to the `SDCard` object
 * \param file A reference to the `FIL` object
 * \param data A reference to the `data_sample` object containing the sensor readings and elapsed time.
 *
 * \return void
 */
void write_csv_row(SDCard &sd_card, FIL &file, struct data_sample &data);
