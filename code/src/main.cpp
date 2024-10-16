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
#include "drivers/dac/MCP4716.h"
#include "hardware/i2c.h" // should be in the class only
#include <string>
#include <math.h>

#define ALPHA 0.01f // Define the smoothing factor for the load cell

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

int main()
{
    // Initialize stdio and peripherals
    stdio_init_all();

    MCP4716 dac;

    dac.init();
    dac.set_vref(MCP4716::VDD);
    dac.set_gain(MCP4716::ONE);
    dac.set_power_down(MCP4716::NORMAL);

    float voltage = 0.0f;
    const float step = 0.5f;
    bool increasing = true;

    while (true) {
        // Set the voltage
        dac.set_voltage(voltage);

        // Adjust voltage based on direction
        if (increasing) {
            voltage += step;
            if (voltage >= 5.0f) {
                voltage = 5.0f;
                increasing = false;  // Start decreasing
            }
        } else {
            voltage -= step;
            if (voltage <= 0.0f) {
                voltage = 0.0f;
                increasing = true;   // Start increasing
            }
        }
        sleep_ms(1000);
}
}
