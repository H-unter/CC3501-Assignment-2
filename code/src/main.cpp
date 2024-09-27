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

#include <string>
#include <math.h>

int main()
{
    stdio_init_all();

    // Create a LoadCell object
    LoadCell loadcell;
    loadcell.init(26, ALPHA); // Initialize ADC and smoothing factor

    while (true)
    {
        float mass_kg = loadcell.sample_mass(); // Sample mass
        printf("Mass: %f kg\n", mass_kg);
        sleep_ms(1); // Sleep for 1 millisecond
    }

    return 0;
}
