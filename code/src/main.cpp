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
#include "hardware/i2c.h" // should be in the class only
#include <string>
#include <math.h>

#define ALPHA 0.01f // Define the smoothing factor for the load cell
// Define SDA and SCL pins for I2C1 using #define
#define SDA_PIN 7
#define SCL_PIN 6

// Main function
int main()
{
    stdio_init_all();

    // Initialize DAC object
    DAC dac;

    // Initialize I2C and DAC
    dac.init(); // Uses page 44 for I2C initialization info

    // Set output voltage to 2.5V
    printf("[Main] Setting output voltage to 2.5V.\n");
    dac.set_voltage(2.5f); // Voltage is scaled and split based on page 49

    // Done
    printf("[Main] Finished setting voltage.\n");
    return 0;
}