#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/time.h"
// RP2040 hardware includes
#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

// SD card driver
#include "ff.h"        // FatFs file system library
#include "hw_config.h" // Include your custom hardware config (SPI and SD card)

#include "board.h"      // contains constants for the board
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"

#define FILENAME_BUFFER_SIZE 128

// Declare the file system object
FATFS fs;

// Declare the file object
FIL file;

// Buffer for reading/writing data
char buffer[64];

// Utility function to format the current date and time into strings
void get_current_datetime(char *date, char *time)
{
    datetime_t t;
    rtc_get_datetime(&t); // Get current time from RTC

    // Format the date as YYYY-MM-DD and the time as HH
    snprintf(date, FILENAME_BUFFER_SIZE, "%04d-%02d-%02d", t.year, t.month, t.day);
    snprintf(time, FILENAME_BUFFER_SIZE, "%02d", t.hour);
}

// Function to initialize the temperature sensor
void init_temperature_sensor()
{
    adc_init();
    adc_set_temp_sensor_enabled(true); // Enable temperature sensor
    adc_select_input(4);               // Select temperature sensor ADC channel (ADC4)
}

// Function to read temperature from the RP2040 internal sensor
float read_temperature()
{
    const float conversion_factor = 3.3f / (1 << 12); // 12-bit ADC
    uint16_t raw = adc_read();
    float voltage = raw * conversion_factor;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

int main()
{
    stdio_init_all();          // Initialize stdio for printf
    init_temperature_sensor(); // Initialize temperature sensor
    rtc_init();                // Initialize the RTC

    FRESULT fr; // FatFs return code

    // Initialize the SD card driver (handled by the repo's SD driver)
    sd_init_driver();

    // Mount the file system
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK)
    {
        printf("Failed to mount file system! Error: %d\n", fr);
        while (true)
            ; // Halt if the SD card can't be mounted
    }

    printf("File system mounted successfully!\n");

    while (true)
    {
        // Get current date and time strings
        char date[FILENAME_BUFFER_SIZE], hour[FILENAME_BUFFER_SIZE];
        get_current_datetime(date, hour);

        // Build the file path using date and hour, e.g., /data/2021-03-21/11.csv
        char filepath[FILENAME_BUFFER_SIZE];
        snprintf(filepath, FILENAME_BUFFER_SIZE, "/data/%s/%s.csv", date, hour);

        // Create the directory if it doesn't exist
        fr = f_mkdir(filepath);
        if (fr != FR_OK && fr != FR_EXIST)
        {
            printf("Failed to create directory! Error: %d\n", fr);
        }

        // Open the CSV file for appending temperature data
        fr = f_open(&file, filepath, FA_OPEN_APPEND | FA_WRITE);
        if (fr != FR_OK)
        {
            printf("Failed to open file! Error: %d\n", fr);
            while (true)
                ; // Halt if the file can't be opened
        }

        // Read the current temperature
        float temperature = read_temperature();

        // Get the current time for the log entry
        char time[FILENAME_BUFFER_SIZE];
        datetime_t t;
        rtc_get_datetime(&t);
        snprintf(time, FILENAME_BUFFER_SIZE, "%02d:%02d:%02d", t.hour, t.min, t.sec);

        // Prepare the log entry (time and temperature)
        snprintf(buffer, sizeof(buffer), "%s,%.2f\n", time, temperature);

        // Write the log entry to the file
        UINT bytesWritten;
        fr = f_write(&file, buffer, strlen(buffer), &bytesWritten);
        if (fr != FR_OK)
        {
            printf("Failed to write to file! Error: %d\n", fr);
        }
        else
        {
            printf("Logged data: %s", buffer); // Print log entry to console
        }

        // Close the file
        f_close(&file);

        // Sleep for 1 second before taking the next temperature reading
        sleep_ms(1000);
    }

    // Unmount the file system before exiting (though we'll never reach this in this loop)
    f_unmount("");

    return 0;
}
