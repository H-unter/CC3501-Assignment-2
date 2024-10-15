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

    void clear_terminal();
    void reset_buffer();
    bool handle_character_input(char input_character);
    uint16_t handle_command_input();
    char *get_terminal_buffer() { return terminal_buffer; }

private:
    char terminal_buffer[256];
    int terminal_buffer_index;
};

#endif // TERMINAL_H
