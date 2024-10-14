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
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

char terminal_buffer[256] = {0};
int terminal_buffer_index = 0;

int main()
{
    stdio_init_all();

    while (true)
    {
        int input_character = getchar_timeout_us(0);
        bool is_character_recieved = input_character != PICO_ERROR_TIMEOUT;
        if (is_character_recieved)
        {
            terminal_buffer[terminal_buffer_index] = (char)input_character;
            terminal_buffer_index++;
            printf("%c", (char)input_character);
            bool is_enter_recieved = (char)input_character == '\r';
            if (is_enter_recieved)
            {
                terminal_buffer[terminal_buffer_index] = '\0';
                printf("\033[2J\033[H");
                terminal_buffer_index = 0; // Reset the buffer index
            }
        }
    }
}
