#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <cstring>
#include <cstdlib> // For strtof
#include <string>
#include "pico/stdlib.h"
#include "pico/time.h"

/*!
 * \brief Terminal class for handling user input and executing commands.
 *
 * The Terminal class processes character input from the user, interprets commands,
 * and manages a buffer for multi-character commands. A command in this context is represented
 * as a data structure (a struct) called `Command`, which encapsulates the command type
 * and an optional floating-point argument.
 *
 * The `Command` struct contains:
 * - The type of command issued by the user (e.g., `help`, `set_voltage`, `get_data`, `shutdown`).
 * - An optional argument for commands that require additional information, such as a voltage value for `set_voltage`.
 *
 * This class provides methods for:
 * - Handling input character by character.
 * - Parsing and executing user commands by interpreting the `Command` structure.
 * - Resetting and managing the internal command buffer used for building commands from user input.
 */
class Terminal
{
public:
    Terminal();

    /*!
     * \brief Represents a command received from the terminal input.
     *
     * This structure is used to handle commands input by the user through the terminal.
     * It consists of a command type and optional arguments provided by the user, which
     * can include both a numeric and a string-based argument for specific commands.
     *
     * \param command_name The type of command issued by the user.
     * \param numeric_argument A floating-point number used as an argument for commands that require numeric input (e.g., voltage).
     * \param string_argument A string argument or character data used for commands that involve string-based input (e.g., SDI-12 command).
     */
    struct Command
    {
        enum command_name_t
        {
            unrecognised = 0, /*!< Unrecognized or invalid command */
            help,             /*!< Displays available commands */
            set_voltage,      /*!< Sets the DAC voltage (requires numeric_argument) */
            sdi12_send,       /*!< Sends an SDI-12 command (requires string_argument) */
            get_data,         /*!< Retrieves current sensor data */
            shutdown          /*!< Unmounts SD card and shuts down the system */
        } command_name;       /*!< Stores the command type issued by the user. */

        float numeric_argument;      /*!< A floating-point number for commands requiring numeric input (e.g., voltage). */
        std::string string_argument; /*!< A string argument for commands that require a textual input (e.g., SDI-12 command). */
    };

    void clear_terminal();
    void reset_buffer();
    bool handle_character_input(char input_character);
    Command handle_command_input();
    char *get_terminal_buffer() { return terminal_buffer; }

private:
    static const int terminal_buffer_size = 256;
    char terminal_buffer[terminal_buffer_size];
    int terminal_buffer_index;

    Command::command_name_t parse_command(const char *input);
    float parse_float(const char *input);
};

#endif // TERMINAL_H