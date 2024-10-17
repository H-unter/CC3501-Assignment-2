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
#include "drivers/terminal/terminal.h"
#include "drivers/dac/MCP4716.h"

#include "hardware/i2c.h" // should be in the class only
#include <string>
#include <math.h>

#define ALPHA 0.01f // Define the smoothing factor for the load cell
#include "pico/binary_info.h"
#include "hardware/i2c.h"


// Function to be called by the timer
bool sample_data(struct repeating_timer *t)
{
    // put data polling here (hardcode which sdi-12 sensors we have)
    // maybe this updates some kind of global data structure, and whenever the user in the terminal asks for the data, a new poll is not rewquired, rather, the most recent sample of data is given to the user

    // GENERAL STRUCTURE
    // sample load cell
    // sample dac (whether we get the input from the dpg or just return the theoretical value we believe we sent)
    // sample the sdi-12 sensors TODO: figure out which commands we have to send in order to do this

    // save this all to the sd card (appen to the end of the .csv file)
    printf("sampling data...\n");
    return true;
}

int main()
{
    // init sampling timer
    struct repeating_timer timer;
    int32_t polling_rate_ms = 5000;
    add_repeating_timer_ms(polling_rate_ms, sample_data, NULL, &timer);

    // init terminal
    Terminal terminal;

    // init dac
    // MCP4716 dac;
    // dac.init();
    // dac.set_vref(MCP4716::VDD);
    // dac.set_gain(MCP4716::ONE);
    // dac.set_power_down(MCP4716::NORMAL);

    while (true)
    {
        int input_character = getchar_timeout_us(0);
        if (input_character == PICO_ERROR_TIMEOUT)
        {
            continue;
        }

        bool is_command_ready = terminal.handle_character_input((char)input_character);
        if (!is_command_ready)
        {
            continue;
        }

        // Handle the input and get the command + argument
        Terminal::Command result = terminal.handle_command_input();
        switch (result.command_name)
        {
        case Terminal::Command::unrecognised:
            printf("> Unrecognised/invalid command\n");
            break;
        case Terminal::Command::help:
            printf("> Help command \n> set_voltage - sets voltage value from 0-5V\n> get_data - gets the data from the sensors\n");
            break;
        case Terminal::Command::set_voltage:
            printf("> Set voltage command to %.2fV\n", result.argument);
            // dac.set_voltage(result.argument); // Use the voltage argument
            break;
        case Terminal::Command::get_data:
            printf("> Get data command\n");
            break;
        default:
            break;
        }

        terminal.reset_buffer();
    }

    return 0;
