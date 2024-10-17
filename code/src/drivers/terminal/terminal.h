#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <cstring>
#include <cstdlib> // For strtof
#include "pico/stdlib.h"
#include "pico/time.h"

class Terminal
{
public:
    Terminal();

    // Struct to store both the command and its argument
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