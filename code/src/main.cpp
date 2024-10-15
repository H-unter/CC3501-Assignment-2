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

// Function to be called by the timer
bool sample_data(struct repeating_timer *t)
{
    // put data polling here (hardcode which sdi-12 sensors we have)
    printf("hello\n");
    return true;
}

int main()
{
    // Initialize the timer to call sample_data every 1000ms
    struct repeating_timer timer;
    int32_t polling_rate_ms = 1000; // adjustable in code
    add_repeating_timer_ms(polling_rate_ms, sample_data, NULL, &timer);

    Terminal terminal; // The class has an internal buffer to handle input characters.
    while (true)
    {
        int input_character = getchar_timeout_us(0);
        if (input_character == PICO_ERROR_TIMEOUT)
        {
            continue; // go to the start of the loop
        }
        bool is_command_ready = terminal.handle_character_input((char)input_character);
        if (!is_command_ready)
        {
            continue; // go to the start of the loop
        }

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
            // dac.set_voltage(3.2f);
            break;
        case Terminal::Command::GetData:
            printf("> Get data command\n");
            // call the sample_data function which is called on the sd card loop
            break;
        default:
            break;
        }

        // Reset the buffer after processing the command
        terminal.reset_buffer();
    }

    return 0;
}