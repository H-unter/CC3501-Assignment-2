// standard libraries
#include <stdio.h>
#include <string>
#include <math.h>

// pico/imported libraries
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
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
static SDCard sd_card;                               // global
static FIL file;                                     // global
static int is_data_logged = true;                    // flag checking if the most recently sampled data has been saved to the sd card
static bool sdi12_s0_start_measurement_flag = false; // sensor 0
static bool sdi12_s0_end_measurement_flag = false;   // sensor 0
static bool sdi12_s1_start_measurement_flag = false; // sensor 1
static bool sdi12_s1_end_measurement_flag = false;   // sensor 1

uint64_t s0_measurement_start_time;
uint64_t s1_measurement_start_time;
int measurement_wait_time_s = 1;

// Arguments to be passed into the repeating timer function
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
    adc_select_input(DPG_ADC_PIN - 26);

    uint16_t dpg_voltage_input = adc_read() * (3.3f / (1 << 12));
    most_recent_data.dpg_voltage = dpg_voltage_input;
    // Set flags to sample the sdi12 bus when the sdi12 bus is ready
    sdi12_s0_start_measurement_flag = true;
    sdi12_s1_start_measurement_flag = true;
    // Record elapsed time (in ms) since boot
    uint64_t current_time_ms = to_ms_since_boot(get_absolute_time());
    most_recent_data.elapsed_time_ms = current_time_ms - start_time_ms;
    is_data_logged = false; // save to sd card when the program is ready
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

        // Start sdi12 s0 measurement
        if (sdi12_s0_start_measurement_flag == true && !sdi12.is_dataline_busy && !sdi12_s0_end_measurement_flag)
        {
            sdi12.set_data_line_driven(true);
            sdi12.send_break();              // Send break
            sdi12.send_command("0M!", true); // Start measurement with sensor address "0"

            // Reset start flag and set end flag for sdi12 s0 after receiving response
            std::string command_response = sdi12.receive_command_blocking();
            bool is_valid_start_measurement_start_response = command_response.length() == 5; // expected response is 000<CR><LF>
            if (is_valid_start_measurement_start_response)
            {
                s0_measurement_start_time = to_ms_since_boot(get_absolute_time());
                sdi12_s0_start_measurement_flag = false;
                sdi12_s0_end_measurement_flag = true;
            }
        }

        // Record elapsed time (in ms) since boot
        int64_t s0_measurement_elapsed_time_ms = to_ms_since_boot(get_absolute_time()) - s0_measurement_start_time;
        float s0_measurement_elapsed_time_s = float(s0_measurement_elapsed_time_ms) / 1000.0f;
        bool is_s0_measurement_ready = (sdi12_s0_end_measurement_flag && s0_measurement_elapsed_time_s >= measurement_wait_time_s);

        // Start sdi12 s1 measurement
        if (sdi12_s1_start_measurement_flag == true && !sdi12.is_dataline_busy && !sdi12_s1_end_measurement_flag)
        {
            sdi12.set_data_line_driven(true);
            sdi12.send_break();              // Send break
            sdi12.send_command("1M!", true); // Start measurement with sensor address "1"

            // Reset start flag and set end flag for sdi12 s1 after receiving response
            std::string command_response = sdi12.receive_command_blocking();
            bool is_valid_start_measurement_start_response = command_response.length() == 5; // expected response is 000<CR><LF>
            if (is_valid_start_measurement_start_response)
            {
                s1_measurement_start_time = to_ms_since_boot(get_absolute_time());
                sdi12_s1_start_measurement_flag = false;
                sdi12_s1_end_measurement_flag = true;
            }
        }

        // Record elapsed time (in ms) since boot
        int64_t s1_measurement_elapsed_time_ms = to_ms_since_boot(get_absolute_time()) - s1_measurement_start_time;
        float s1_measurement_elapsed_time_s = float(s1_measurement_elapsed_time_ms) / 1000.0f;
        bool is_s1_measurement_ready = (sdi12_s1_end_measurement_flag && s1_measurement_elapsed_time_s >= measurement_wait_time_s);

        if (is_s0_measurement_ready)
        {
            sdi12.set_data_line_driven(true);
            sdi12.send_break();               // Send break
            sdi12.send_command("0D0!", true); // Send Data Command with sensor address "0"
            std::string measurement_response = sdi12.receive_command_blocking(); // Get measurement

            // If response is valid, parse calue from response and reset end flag for sdi12 s0 
            bool is_valid_measurement_response = (measurement_response.find('+') != std::string::npos);
            if (is_valid_measurement_response)
            {
                float measurement = sdi12.parse_value_from_response(measurement_response);
                most_recent_data.leaf_temperature = measurement;
                sdi12_s0_end_measurement_flag = false;
            }
        }

        if (is_s1_measurement_ready)
        {
            sdi12.set_data_line_driven(true);
            sdi12.send_break();               // Send break
            sdi12.send_command("1D0!", true); // Send Data Command with sensor address "1"
            std::string measurement_response = sdi12.receive_command_blocking(); // Get measurement

            // If response is valid, parse calue from response and reset end flag for sdi12 s1
            bool is_valid_measurement_response = (measurement_response.find('+') != std::string::npos);
            if (is_valid_measurement_response)
            {
                float measurement = sdi12.parse_value_from_response(measurement_response);
                most_recent_data.sap_flow = measurement;
                sdi12_s1_end_measurement_flag = false;
            }
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

        // At this point in the program a command is ready to be processed and handled
        // Handle the input and get the command + argument
        Terminal::Command result = terminal.handle_command_input();
        switch (result.command_name)
        {
        // If command is unrecognised
        case Terminal::Command::unrecognised:
            printf("> Unrecognised/invalid command\n");
            break;
        // Help command
        case Terminal::Command::help:
            printf("> Help command \n> set_voltage - sets voltage value from 0-5V\n> get_data - gets the data from the sensors\n> sdi12_send - send whatever you want to the sdi12 bus\n> shutdown - safely shutdown the board\n");
            break;
        // Command to manually set voltage of Dew Point Generator
        case Terminal::Command::set_voltage:
        {
            float input_voltage = result.numeric_argument;
            printf("> Setting voltage to %.2fV\n", input_voltage);
            dac.set_voltage(input_voltage);
            most_recent_data.dac_voltage = input_voltage;
            break;
        }
        // Command to manually send SDI-12 commands
        case Terminal::Command::sdi12_send:
        {
            std::string input_command = result.string_argument;
            sdi12.set_data_line_driven(true);
            sdi12.send_break();
            sdi12.send_command(input_command, true);
            printf("> Sending SDI-12 command: %s\n", input_command.c_str());
            std::string command_response = sdi12.receive_command_blocking();
            printf("> Received %d characters: %s\n", command_response.length(), command_response.c_str());
            break;
        }
        // Command to get data
        case Terminal::Command::get_data:
            printf("> Get data command\n");
            printf("Elapsed Time: %llu ms\n", most_recent_data.elapsed_time_ms);
            printf("Plant Mass: %f kg\n", most_recent_data.loadcell_weight);
            printf("Loadcell Voltage: %f V\n", most_recent_data.loadcell_voltage);
            printf("DAC input Voltage: %f V\n", most_recent_data.dac_voltage);
            printf("DPG Voltage: %f V\n", most_recent_data.dpg_voltage);
            printf("Leaf Temperature: %f C\n", most_recent_data.leaf_temperature);
            printf("Sap Flow: %f ml/hour\n", most_recent_data.sap_flow);
            break;
        // Shutdown command
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