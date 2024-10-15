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
#include "drivers/terminal/terminal.h"
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
    Terminal terminal; // put a comment about having an internal buffer in the class

    while (true)
    {
        int input_character = getchar_timeout_us(0);
        if (input_character != PICO_ERROR_TIMEOUT)
        {
            bool is_command_ready = terminal.handle_character_input((char)input_character);
            if (is_command_ready)
            {
                uint16_t result = terminal.handle_command_input();
                switch (result)
                {
                case 0: // unrecognised command
                    printf("> Unrecognised command\n");
                    break;
                case 1: // help
                    printf("> Help command \n\r> useful stuff here\n");
                    break;
                case 2: // set_voltage
                    printf("> Set voltage command\n");
                    break;
                case 3: // get_data
                    printf("> Get data command\n");
                    break;
                default:
                    break;
                }
                terminal.reset_buffer();
            }
        }
    }

    return 0;
}
