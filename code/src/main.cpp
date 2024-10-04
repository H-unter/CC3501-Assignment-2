#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "pico/time.h" // Include time functions

#include "board.h"      // Contains constants for the board
#include "WS2812.pio.h" // WS2812 driver
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"
#include "drivers/loadcell/loadcell.h"
#include "drivers/sd_card/sd_card.h"
#include "ff.h" // For 'FIL' definition
#include "drivers/dac/dac.h"

#include <string>
#include <math.h>

#define ALPHA 0.01f // Define the smoothing factor for the load cell

// Function to write the CSV header
void write_csv_header(SDCard &sd_card, FIL &file)
{
    char header_buffer[64];
    snprintf(header_buffer, sizeof(header_buffer), "elapsed_time_ms,mass_loadcell0\n");
    if (!sd_card.write_file_sync(file, header_buffer))
    {
        printf("Failed to write header\n");
    }
}

// Function to save a single sample
void save_sample(SDCard &sd_card, FIL &file, LoadCell &loadcell, uint64_t elapsed_time_ms)
{
    // Sample mass from the load cell
    float mass_kg = loadcell.sample_mass();
    printf("Mass: %.2f kg\n", mass_kg);

    // Prepare data for logging
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%llu,%.2f\n", elapsed_time_ms, mass_kg);

    // Write data to the file
    if (!sd_card.write_file_sync(file, buffer))
    {
        printf("Failed to write data\n");
    }
}

uint32_t get_elapsed_time_ms(uint64_t start_time_ms)
{
    uint64_t current_time_ms = to_ms_since_boot(get_absolute_time());
    return current_time_ms - start_time_ms;
}

int main()
{
    // Initialize stdio and peripherals
    stdio_init_all();

    DAC dpg;

    dpg.init();
    while(true){
        dpg.set_voltage(5);
        sleep_ms(10);
    }

}
