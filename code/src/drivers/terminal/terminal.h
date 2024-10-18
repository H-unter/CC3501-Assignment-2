#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <cstring>
#include <cstdlib> // For strtof
#include "pico/stdlib.h"
#include "pico/time.h"

/*!
 * \brief Terminal class for handling user input and executing commands.
 *
 * The Terminal class processes character input from the user, interprets commands,
 * and manages a buffer to handle multi-character commands. It supports various commands such as
 * `set_voltage`, `get_data`, and `shutdown`, and allows for an optional floating-point argument
 * to be passed along with certain commands.
 *
 * This class provides methods for:
 * - Handling input character by character.
 * - Parsing and executing user commands.
 * - Resetting and managing the internal command buffer.
 */
class Terminal
{
public:
    Terminal();

    /*!
     * \brief Represents a command received from the terminal input.
     *
     * This structure is used to handle commands input by the user through the terminal.
     * It consists of a command type and an optional floating-point argument provided by the user,
     * which is used with specific commands, such as `set_voltage`.
     *
     * \param command_name The type of command issued by the user.
     * \param argument A floating-point value that accompanies certain commands (e.g., `set_voltage`). It has a default value of 0, for commands that do not require an argument.
     */
    struct Command
    {
        enum command_name_t
        {
            unrecognised = 0,
            help,
            set_voltage,
            get_data,
            shutdown
        } command_name;

        float argument; // To store the floating-point number (e.g., voltage)
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