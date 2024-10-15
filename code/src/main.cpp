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
    Terminal terminal; // The class has an internal buffer to handle input characters.
    while (true)
    {
        int input_character = getchar_timeout_us(0);

        // Continue if there is a timeout
        if (input_character == PICO_ERROR_TIMEOUT)
        {
            continue;
        }

        // Process the character input
        bool is_command_ready = terminal.handle_character_input((char)input_character);

        // If the command is not ready, continue
        if (!is_command_ready)
        {
            continue;
        }

        // Handle the command
        Terminal::Command result = terminal.handle_command_input();
        switch (result)
        {
        case Terminal::Command::Unrecognised:
            printf("> Unrecognised command\n");
            break;
        case Terminal::Command::Help:
            printf("> Help command \n\r> useful stuff here\n");
            break;
        case Terminal::Command::SetVoltage:
            printf("> Set voltage command\n");
            break;
        case Terminal::Command::GetData:
            printf("> Get data command\n");
            break;
        default:
            break;
        }

        // Reset the buffer after processing the command
        terminal.reset_buffer();
    }

    return 0;
}