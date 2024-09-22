#include <stdio.h>
#include <string.h>
#include <time.h> // Include time library for time-related functions

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "ff.h"         // FatFs file system library
#include "hw_config.h"  // Include your custom hardware config (SPI and SD card)
#include "board.h"      // Contains constants for the board
#include "WS2812.pio.h" // WS2812 driver
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"
#include "drivers/sd_card/sd_card.h"

// Function to read temperature from ADC and return it
// Function to enable the temperature sensor bias and read temperature from ADC
float read_temperature()
{
    adc_init();                        // Initialize ADC system
    adc_set_temp_sensor_enabled(true); // Enable temperature sensor bias
    adc_select_input(4);               // Select ADC input 4 (temperature sensor)

    sleep_ms(10); // Short delay to allow the sensor to stabilize

    uint16_t raw_value = adc_read();                  // Read raw ADC value
    const float conversion_factor = 3.3f / (1 << 12); // 12-bit resolution
    float voltage = raw_value * conversion_factor;    // Convert ADC value to voltage

    // Calculate temperature using the RP2040's formula
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;

    adc_set_temp_sensor_enabled(false); // Disable temperature sensor to save power

    return temperature; // Return calculated temperature
}

// Function to log temperature to the SD card
void log_temperature(SDCard &sd_card, FIL &file, int count)
{
    for (int i = 0; i < count; i++)
    {
        // Read temperature
        float temperature = read_temperature();
        printf("Temperature = %.2f °C\n", temperature);

        // Prepare data for logging
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Temperature = %.2f °C\n", temperature);

        // Write data to the file
        if (!sd_card.write_file_sync(file, buffer))
        {
            printf("Failed to write data\n");
        }

        // Wait for some time before logging next data point
        sleep_ms(2000); // 2 seconds delay for the next temperature reading
    }
}

int main()
{
    stdio_init_all();
    adc_init(); // Initialize ADC for temperature reading

    // Create an instance of the SDCard class
    SDCard sd_card;

    // Mount the SD card
    if (sd_card.mount())
    {
        printf("SD card mounted successfully.\n");

        // Open file for logging
        FIL file;
        if (sd_card.open_file("temperature_log.csv", file))
        {
            printf("File opened for logging.\n");

            // Log 3 temperature data points
            log_temperature(sd_card, file, 3);

            // Close the file and unmount the SD card
            sd_card.close_file(file);
            sd_card.unmount();
            printf("Logging finished. SD card unmounted.\n");
        }
        else
        {
            printf("Failed to open log file.\n");
        }
    }
    else
    {
        printf("Failed to mount SD card.\n");
    }

    return 0;
}