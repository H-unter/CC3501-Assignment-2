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

int main()
{
    stdio_init_all(); // Initialize standard IO

    // Create an instance of the SDCard class
    SDCard sd_card;

    // Mount the SD card
    if (sd_card.mount())
    {
        printf("SD card mounted successfully.\n");

        // List files in the root directory "/"
        printf("Listing files in the root directory:\n");
        std::vector<std::string> files = sd_card.list_files("/data");
        for (const auto &file : files)
        {
            printf("Found: %s\n", file.c_str());
        }

        // Write some data to a file
        const std::string filename = "log.txt";
        const std::string data = "Hello, SD card logging!\n";

        if (sd_card.write_file(filename, data))
        {
            printf("Data written successfully to %s.\n", filename.c_str());
        }
        else
        {
            printf("Failed to write data to %s.\n", filename.c_str());
        }

        // Unmount the SD card
        sd_card.unmount();
        printf("SD card unmounted successfully.\n");
    }
    else
    {
        printf("Failed to mount SD card.\n");
    }

    return 0;
}