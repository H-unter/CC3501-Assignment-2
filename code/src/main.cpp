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
#include "my_debug.h" // Include for debug utilities

// Function declarations
bool mount_sd_card(sd_card_t **pSD);
bool open_log_file(FIL *fp);
bool log_temperature(FIL *fp);
bool print_header(FIL *fp);

void list_files(const char *directory)
{
    DIR dir;     // Directory object
    FILINFO fno; // File information object
    FRESULT res;

    // Open the directory
    res = f_opendir(&dir, directory);
    if (res != FR_OK)
    {
        printf("Failed to open directory: %s, error code: %d\n", directory, res);
        return;
    }

    // Read files and directories
    while (true)
    {
        res = f_readdir(&dir, &fno); // Read a directory item
        if (res != FR_OK || fno.fname[0] == 0)
        {
            break; // Break on error or end of directory
        }

        if (fno.fattrib & AM_DIR)
        {
            printf("[DIR] %s\n", fno.fname); // It's a directory
        }
        else
        {
            printf("[FILE] %s (%lu bytes)\n", fno.fname, fno.fsize); // It's a file
        }
    }

    f_closedir(&dir); // Close the directory
}

void set_hardcoded_rtc()
{
    datetime_t t = {
        .year = 2024,
        .month = 9,
        .day = 18,
        .dotw = 3, // Wednesday (Sun=0, Mon=1, Tue=2, etc.)
        .hour = 14,
        .min = 30,
        .sec = 0};

    rtc_init();
    if (rtc_set_datetime(&t) != 0)
    {
        printf("Failed to set RTC time!\n");
    }
    else
    {
        printf("RTC set successfully: %d-%02d-%02d %02d:%02d:%02d\n", t.year, t.month, t.day, t.hour, t.min, t.sec);
    }
}

int main()
{
    stdio_init_all(); // Initialize all standard IO

    puts("Starting system...");

    // Mount the SD card
    sd_card_t *pSD;
    if (!mount_sd_card(&pSD))
    {
        puts("Failed to mount SD card!");
        return -1;
    }
    puts("SD card mounted successfully.");
    list_files("/"); // List files in the root directory of the SD card
    // Set the RTC with a hardcoded date and time
    set_hardcoded_rtc();

    FIL fil;
    if (!open_log_file(&fil))
    {
        puts("Failed to open log file.");
        return -1;
    }

    // Loop to log temperature 3 times
    for (int i = 0; i < 3; i++)
    {
        if (!log_temperature(&fil))
        {
            puts("Failed to log temperature data!");
        }
        else
        {
            printf("Temperature data logged successfully (%d/3).\n", i + 1);
        }
        // Delay 1 second between data logs for demonstration
        sleep_ms(1000);
    }

    // Close the log file after all logs
    f_close(&fil);

    // Unmount the SD card
    f_unmount(pSD->pcName);

    puts("Finished logging data. Goodbye!");
    return 0;
}

// Function to mount the SD card
bool mount_sd_card(sd_card_t **pSD)
{
    printf("Attempting to mount SD card...\n");
    *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&(*pSD)->fatfs, (*pSD)->pcName, 1);
    if (FR_OK != fr)
    {
        printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }
    return true;
}

// Open or create the file with a structured name based on current date and time
bool open_log_file(FIL *fp)
{
    datetime_t datetime;
    rtc_get_datetime(&datetime); // Get the current time from RTC

    char filename[64];

    printf("Creating or opening file for logging...\n");

    // Create the root-level "data" directory (relative to SD card root)
    snprintf(filename, sizeof(filename), "0:/data");
    FRESULT fr = f_mkdir(filename);
    if (FR_OK != fr && FR_EXIST != fr)
    {
        printf("f_mkdir error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    // Create the subdirectory for the current date
    snprintf(filename, sizeof(filename), "0:/data/%04d-%02d-%02d",
             datetime.year, datetime.month, datetime.day);
    fr = f_mkdir(filename);
    if (FR_OK != fr && FR_EXIST != fr)
    {
        printf("f_mkdir error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    // Create or open the file for the current hour
    snprintf(filename + strlen(filename), sizeof(filename) - strlen(filename), "/%02d.csv", datetime.hour);
    fr = f_open(fp, filename, FA_OPEN_APPEND | FA_WRITE);
    if (FR_OK != fr)
    {
        printf("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
        return false;
    }

    printf("File opened: %s\n", filename);

    // Print the header if the file is empty
    return print_header(fp);
}

// Function to log temperature to a CSV file
bool log_temperature(FIL *fp)
{
    char buf[128];
    datetime_t datetime;
    rtc_get_datetime(&datetime); // Use RTC for current time
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d,%02d:%02d:%02d,",
             datetime.year, datetime.month, datetime.day,
             datetime.hour, datetime.min, datetime.sec);

    printf("Logging temperature data...\n");

    // Initialize the ADC (in case it's not initialized elsewhere)
    adc_init();
    adc_gpio_init(28);   // Initialize GPIO pin 28 for ADC input
    adc_select_input(4); // Select ADC input 4

    // Read the temperature from ADC input 4
    uint16_t result = adc_read();
    printf("ADC raw value: %d\n", result); // Print the raw ADC value
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = conversion_factor * result;
    printf("Calculated voltage: %.3f V\n", voltage); // Print the voltage

    // Calculate temperature in °C
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    printf("Calculated temperature: %.3f °C\n", temperature); // Print the temperature
    int nw = snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%.3g\n", (double)temperature);
    myASSERT(0 < nw && nw < (int)sizeof buf);

    // Write the temperature data to the file
    printf("Writing data to file...\n");
    if (f_printf(fp, "%s", buf) < 0)
    {
        printf("f_printf failed\n");
        return false;
    }

    // Flush the file buffers to ensure data is written to the SD card
    printf("Flushing data to SD card...\n");
    FRESULT fr = f_sync(fp);
    if (FR_OK != fr)
    {
        printf("f_sync error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    return true;
}

// Print the header to the CSV file if it's empty
bool print_header(FIL *fp)
{
    FRESULT fr = f_lseek(fp, f_size(fp));
    if (FR_OK != fr)
    {
        printf("f_lseek error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    // Write the header if the file is new
    if (0 == f_tell(fp))
    {
        if (f_printf(fp, "Date,Time,Temperature (°C)\n") < 0)
        {
            printf("f_printf error\n");
            return false;
        }
        printf("Header written to file.\n");
    }
    return true;
}
