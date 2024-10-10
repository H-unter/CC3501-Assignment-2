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


// Sweep through all 7-bit I2C addresses, to see if any slaves are present on
// the I2C bus. Print out a table that looks like this:
//
// I2C Bus Scan
//    0 1 2 3 4 5 6 7 8 9 A B C D E F
// 00 . . . . . . . . . . . . . . . .
// 10 . . @ . . . . . . . . . . . . .
// 20 . . . . . . . . . . . . . . . .
// 30 . . . . @ . . . . . . . . . . .
// 40 . . . . . . . . . . . . . . . .
// 50 . . . . . . . . . . . . . . . .
// 60 . . . . . . . . . . . . . . . .
// 70 . . . . . . . . . . . . . . . .
// E.g. if addresses 0x12 and 0x34 were acknowledged.

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main() {
    // Enable UART so we can print status output
    stdio_init_all();
#if !defined(DAC_I2C_INSTANCE) || !defined(SDA_PIN) || !defined(SCL_PIN)
#warning i2c/bus_scan example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(DAC_I2C_INSTANCE, I2C_BAUD_RATE);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    //gpio_pull_up(SDA_PIN);
    //gpio_pull_up(SCL_PIN);
    // Make the I2C pins available to picotool
    //bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(DAC_I2C_INSTANCE, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
    return 0;
#endif
}