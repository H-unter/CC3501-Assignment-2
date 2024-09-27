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

float get_new_average(float newest_sample, float old_average)
{
    return ALPHA * newest_sample + (1.0f - ALPHA) * old_average;
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0); // Select ADC input 0 (GPIO26)
    printf("--------------------\n\n\n\n\n\n");

    float smoothed_voltage = get_adc_voltage();

    while (true)
    {
        float voltage = get_adc_voltage();
        smoothed_voltage = get_new_average(voltage, smoothed_voltage);
        float mass_kg = (3.4548 * smoothed_voltage) - 0.4554;
        printf("%f,%f\n", smoothed_voltage, mass_kg);
        sleep_ms(1);
    }
}
