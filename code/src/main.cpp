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

float ADC_VOLTAGE_CONVERSION_FACTOR = 3.3f / (1 << 12); // pg 79 of SDK datasheet

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0); // Select ADC input 0 (GPIO26)
    printf("--------------------\n");
    while (true)
    {
        float voltage = adc_read() * ADC_VOLTAGE_CONVERSION_FACTOR;
        printf("Voltage: %f\n", voltage);
        sleep_ms(1);
    }
}
