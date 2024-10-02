#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/adc.h"

#include "board.h"      // contains constants for the board
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"
#include "drivers/loadcell/loadcell.h"
#include "drivers/sd_card/sd_card.h"

#include <string>
#include <math.h>

// Function to log load cell data to the SD card
void log_loadcell_data(SDCard &sd_card, FIL &file, LoadCell &loadcell, int count)
{
    char header_buffer[64];
    snprintf(header_buffer, sizeof(header_buffer), "mass_loadcell0,test\n");
    sd_card.write_file_sync(file, header_buffer);

    for (int i = 0; i < count; i++)
    {
        // Sample mass from the load cell
        float mass_kg = loadcell.sample_mass();
        printf("Mass: %.2f kg\n", mass_kg);

        // Prepare data for logging
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.2f, test\n", mass_kg);

        // Write data to the file
        if (!sd_card.write_file_sync(file, buffer))
        {
            printf("Failed to write data\n");
        }

        // Wait for some time before logging the next data point
        sleep_ms(100); // 1-second delay
    }
}

int main()
{
    stdio_init_all();

    // Create a LoadCell object
    LoadCell loadcell;
    loadcell.init(26, ALPHA); // Initialize ADC and smoothing factor

    // Create an instance of the SDCard class
    SDCard sd_card;

    // Mount the SD card
    if (sd_card.mount())
    {
        printf("SD card mounted successfully.\n");

        // Open file for logging
        FIL file;
        if (sd_card.open_file("loadcell_log.csv", file))
        {
            printf("File opened for logging.\n");

            // Log 10 load cell data points
            log_loadcell_data(sd_card, file, loadcell, 10);

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