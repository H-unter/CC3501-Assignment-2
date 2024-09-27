#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/adc.h"

#include "board.h"      // contains constants for the board
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"

#include <string>

#define ADC_VOLTAGE_CONVERSION_FACTOR (3.3f / (1 << 12)) // pg 79 of SDK datasheet
#define ALPHA 0.01f                                      // smoothing

float get_adc_voltage()
{
    return adc_read() * ADC_VOLTAGE_CONVERSION_FACTOR;
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0); // Select ADC input 0 (GPIO26)
    printf("--------------------\n\n\n\n\n\n");

    float moving_average_01 = get_adc_voltage(); // Initialize EMA (can also initialize with the first ADC value if needed)
    float moving_average_05 = get_adc_voltage();
    float moving_average_10 = get_adc_voltage();
    float moving_average_20 = get_adc_voltage();

    while (true)
    {
        float voltage = get_adc_voltage();
        moving_average_01 = 0.01f * voltage + (1.0f - 0.01f) * moving_average_01;
        moving_average_05 = 0.05f * voltage + (1.0f - 0.05f) * moving_average_05;
        moving_average_10 = 0.10f * voltage + (1.0f - 0.10f) * moving_average_10;
        moving_average_20 = 0.20f * voltage + (1.0f - 0.20f) * moving_average_20;
        printf("%f,%f,%f,%f,%f\n", voltage, moving_average_01, moving_average_05, moving_average_10, moving_average_20); // Print both raw and EMA voltage
        sleep_ms(1);
    }
}
