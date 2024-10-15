#include "terminal.h"

Terminal::Terminal()
{
    stdio_init_all();
    reset_buffer();
}

void Terminal::clear_terminal()
{
    printf("\033[2J\033[H");
}

void Terminal::reset_buffer()
{
    for (int i = 0; i < terminal_buffer_index; i++)
    {
        terminal_buffer[i] = 0;
    }
    terminal_buffer_index = 0;
}

// returns true if a command is ready to be processed
bool Terminal::handle_character_input(char input_character)
{
    if (input_character == '\r')
    {                                                  // Enter key pressed
        terminal_buffer[terminal_buffer_index] = '\0'; // Null-terminate
        printf("\r\n");
        return true;
    }
    terminal_buffer[terminal_buffer_index] = input_character;
    terminal_buffer_index++;
    printf("%c", input_character);
    return false;
}

// 0 - unrecognised command
// 1 - help
// 2 - set_dac_voltage
// 3 - get_data (a list of all of the most current sensor data)
uint16_t Terminal::handle_command_input()
{
    if (strcmp(terminal_buffer, "help") == 0)
    {
        return 1;
    }
    else if (strcmp(terminal_buffer, "set_voltage") == 0)
    {
        return 2;
    }
    else if (strcmp(terminal_buffer, "get_data") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
