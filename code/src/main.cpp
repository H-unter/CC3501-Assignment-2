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
bool log_temperature();
bool open_file(FIL *fp);
bool print_header(FIL *fp);

// Structure to hold the initial time data from config.txt
typedef struct
{
    int16_t year;
    int8_t month, day, hour, min, sec, dotw;
} ConfigTime;

int parse_time_config(const char *filename, ConfigTime *config)
{
    printf("Attempting to open file: %s\n", filename);

    FILE *file = fopen("0:/config.txt", "r");
    if (!file)
    {
        printf("Failed to open %s\n", filename);
        return 0;
    }

    printf("File %s opened successfully.\n", filename);

    // Parse the config.txt file
    int scanned = fscanf(file, "time = {\n .year  = %d,\n .month = %d,\n .day   = %d,\n .dotw  = %d,\n .hour  = %d,\n .min   = %d,\n .sec   = %d\n}",
                         &config->year, &config->month, &config->day, &config->dotw, &config->hour, &config->min, &config->sec);

    if (scanned == 7)
    {
        printf("Time config parsed successfully.\n");
    }
    else
    {
        printf("Error parsing config.txt, scanned fields: %d\n", scanned);
    }

    fclose(file);
    return (scanned == 7); // Return 1 if successfully scanned all 7 fields
}

void set_rtc_from_config(ConfigTime config)
{
    printf("Setting RTC from config...\n");
    datetime_t t = {
        .year = config.year,
        .month = config.month,
        .day = config.day,
        .dotw = config.dotw, // Day of the week (0=Sunday, etc.)
        .hour = config.hour,
        .min = config.min,
        .sec = config.sec};

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

    // Initialize the Real-Time Clock (RTC)
    rtc_init();
    ConfigTime startTime;

    puts("Starting system...");

    // Mount the SD card
    sd_card_t *pSD;
    if (!mount_sd_card(&pSD))
    {
        puts("Failed to mount SD card!");
        return -1;
    }
    puts("SD card mounted successfully.");

    DIR dir;
    FILINFO fno;
    FRESULT fr = f_opendir(&dir, "/"); // Open root directory
    if (fr == FR_OK)
    {
        while (true)
        {
            fr = f_readdir(&dir, &fno); // Read directory
            if (fr != FR_OK || fno.fname[0] == 0)
                break; // Break on error or end of directory
            printf("Found file: %s\n", fno.fname);
        }
        f_closedir(&dir);
    }
    else
    {
        printf("Failed to open directory\n");
    }

    // Read time configuration from config.txt
    if (!parse_time_config("config.txt", &startTime))
    {
        printf("Error reading config file\n");
        return 1;
    }

    // Set RTC with the parsed start time
    set_rtc_from_config(startTime);

    // Log the temperature data to the SD card
    if (!log_temperature())
    {
        puts("Failed to log temperature data!");
    }
    else
    {
        puts("Temperature data logged successfully.");
    }

    // Unmount the SD card
    f_unmount(pSD->pcName);

    puts("Finished. Goodbye!");
    while (true)
    {
        tight_loop_contents(); // Loop infinitely to keep the system alive
    }

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

// Function to log temperature to a CSV file
bool log_temperature()
{
    FIL fil; // File object
    printf("Opening file to log temperature...\n");
    bool rc = open_file(&fil); // Open or create the CSV file
    if (!rc)
    {
        printf("Failed to open or create the file.\n");
        return false;
    }

    // Get current time
    char buf[128];
    const time_t secs = time(NULL);
    struct tm tmbuf;
    struct tm *ptm = localtime_r(&secs, &tmbuf);
    size_t n = strftime(buf, sizeof buf, "%F,%T,", ptm);
    myASSERT(n);

    printf("Logging temperature data...\n");

    // Read the temperature from ADC input 4
    adc_select_input(4);
    uint16_t result = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = conversion_factor * result;

    // Calculate temperature in °C
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    int nw = snprintf(buf + n, sizeof buf - n, "%.3g\n", (double)temperature);
    myASSERT(0 < nw && nw < (int)sizeof buf);

    printf("Temperature = %.1f °C\n", temperature);

    // Write the temperature data to the file
    if (f_printf(&fil, "%s", buf) < 0)
    {
        printf("f_printf failed\n");
        f_close(&fil); // Ensure the file is closed
        return false;
    }

    // Close the file
    FRESULT fr = f_close(&fil);
    if (FR_OK != fr)
    {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    return true;
}

// Open or create the file with a structured name based on current date and time
bool open_file(FIL *fp)
{
    const time_t timer = time(NULL);
    struct tm tmbuf;
    localtime_r(&timer, &tmbuf);

    char filename[64];

    printf("Creating or opening file for logging...\n");

    // Create the root-level "data" directory (relative to SD card root)
    snprintf(filename, sizeof(filename), "0:/data"); // Use "0:" as root of SD card
    FRESULT fr = f_mkdir(filename);
    if (FR_OK != fr && FR_EXIST != fr)
    {
        printf("f_mkdir error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    // Create the subdirectory for the current date
    snprintf(filename, sizeof(filename), "0:/data/%04d-%02d-%02d",
             tmbuf.tm_year + 1900, tmbuf.tm_mon + 1, tmbuf.tm_mday);
    fr = f_mkdir(filename);
    if (FR_OK != fr && FR_EXIST != fr)
    {
        printf("f_mkdir error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    // Create or open the file for the current hour
    snprintf(filename + strlen(filename), sizeof(filename) - strlen(filename), "/%02d.csv", tmbuf.tm_hour);
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
