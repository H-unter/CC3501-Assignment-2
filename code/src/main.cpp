// standard libraries
#include <stdio.h>
#include <string>
#include <math.h>

// pico/imported libraries
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h" // should be in the class only
#include "pico/stdlib.h"
#include "pico/time.h" // Include time functions
#include "pico/binary_info.h"
#include "ff.h" // sd card library

// board parameters
#include "board.h" // Contains constants for the board

// custom drivers
#include "WS2812.pio.h" // WS2812 driver
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"
#include "drivers/loadcell/loadcell.h"
#include "drivers/sd_card/sd_card.h"
#include "drivers/terminal/terminal.h"
#include "drivers/dac/MCP4716.h" // dac

static data_sample most_recent_data; // globally accessible most recent sample variable
static uint64_t start_time_ms = to_ms_since_boot(get_absolute_time());
static SDCard sd_card;            // lol global
static FIL file;                  // lol also global
static int is_data_logged = true; // flag checking if the most recently sampled data has been saved to the sd card

// arguments to be passed into the repeating timer function
// TODO: address the sd card errors that occour when trying to write to the file inside this timer function
struct sample_data_args
{
    LoadCell *loadcell;
    MCP4716 *dac;
};

bool sample_data(struct repeating_timer *t)
{
    struct sample_data_args *components = (struct sample_data_args *)t->user_data;
    most_recent_data.loadcell_voltage = components->loadcell->read_voltage();
    most_recent_data.loadcell_weight = components->loadcell->sample_mass();
    // sample the sdi-12 sensors TODO: figure out which commands we have to send in order to do this

    // save this all to the sd card (appen to the end of the .csv file)
    uint64_t current_time_ms = to_ms_since_boot(get_absolute_time());
    most_recent_data.elapsed_time_ms = current_time_ms - start_time_ms;
    is_data_logged = false;
    return true;
}
int main()
{
    // init load cell
    LoadCell loadcell;
    loadcell.init(LOADCELL0_ADC_PIN, ALPHA);

    // init terminal
    Terminal terminal;

    // init dac
    MCP4716 dac;
    dac.init();
    dac.set_vref(MCP4716::VDD);
    dac.set_gain(MCP4716::ONE);
    dac.set_power_down(MCP4716::NORMAL);
    dac.set_voltage(most_recent_data.dac_voltage);

    // init sdi-12
    SDI12 sdi12(SDI12_UART_INSTANCE);

    // init sd card
    if (!sd_card.mount() || !sd_card.open_file("loadcell_log.csv", file))
    {
        printf("Failed to initialize SD card.\n");
        return 0;
    }
    write_csv_header(sd_card, file);

    // init sampling timer
    struct repeating_timer timer;
    int32_t sampling_rate_ms = 100;
    struct sample_data_args sample_data_arguments = {&loadcell, &dac};
    add_repeating_timer_ms(sampling_rate_ms, sample_data, (void *)&sample_data_arguments, &timer);

    while (true)
    {
        if (is_data_logged == false)
        {
            write_csv_row(sd_card, file, most_recent_data);
            is_data_logged = true;
        }

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

        // at this point in the program a command is ready to be processed and handled

        // Handle the input and get the command + argument
        Terminal::Command result = terminal.handle_command_input();
        switch (result.command_name)
        {
        case Terminal::Command::unrecognised:
            printf("> Unrecognised/invalid command\n");
            break;
        case Terminal::Command::help:
            printf("> Help command \n> set_voltage - sets voltage value from 0-5V\n> get_data - gets the data from the sensors\n> sdi12_send - send whatever you want to the sdi12 bus\n> shutdown - safely shutdown the board\n");
            break;
        case Terminal::Command::set_voltage:
        {
            float input_voltage = result.numeric_argument;
            printf("> Setting voltage to %.2fV\n", input_voltage);
            dac.set_voltage(input_voltage);
            most_recent_data.dac_voltage = input_voltage;
            break;
        }
        case Terminal::Command::sdi12_send:
        {
            std::string input_command = result.string_argument;
            printf("> Sending SDI-12 command: %s\n", input_command.c_str());
            sdi12.set_data_line_driven(true);
            sdi12.send_break();
            sdi12.send_command(input_command.c_str(), true);
            absolute_time_t sdi12_start_time = get_absolute_time();
            while (!uart_is_readable(SDI12_UART_INSTANCE) && !sdi12.is_timed_out(sdi12_start_time))
            {
                tight_loop_contents();
            }
            int sdi12_buffer_size = 128; // Internal buffer size for storing received characters
            int buffer_index = 0;        // Reset index for new data reception
            uint8_t ch;
            char message_buffer[sdi12_buffer_size] = {0};
            while (uart_is_readable(SDI12_UART_INSTANCE) && buffer_index < sdi12_buffer_size - 1)
            {
                ch = uart_getc(SDI12_UART_INSTANCE); // Read one character from the UART receive buffer
                message_buffer[buffer_index++] = ch; // Store the character in the internal buffer
                sleep_ms(10);                        // enough of a delay to allow the next character to come through
            }
            message_buffer[buffer_index] = '\0';
            printf("> Received %d characters: %s\n", buffer_index, message_buffer);
            break;
        }
        case Terminal::Command::get_data:
            printf("> Get data command\n");
            printf("Plant Mass: %f kg\n", most_recent_data.loadcell_weight);
            printf("Loadcell Voltage: %f V\n", most_recent_data.loadcell_voltage);
            printf("DAC Voltage: %f V\n", most_recent_data.dac_voltage);
            break;
        case Terminal::Command::shutdown:
            printf("> Shutting down...\n");
            cancel_repeating_timer(&timer);
            sd_card.close_file(file);
            sd_card.unmount();
            return 0;
        default:
            break;
        }

        terminal.reset_buffer();
    }

    return 0;
}