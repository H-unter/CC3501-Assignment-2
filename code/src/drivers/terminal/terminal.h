#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "pico/time.h"

class Terminal
{
public:
    Terminal();

    enum class Command
    {
        Unrecognised = 0,
        Help,
        SetVoltage,
        GetData
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
};

#endif // TERMINAL_H
