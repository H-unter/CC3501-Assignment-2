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
    for (int i = 0; i < terminal_buffer_size; i++) // Iterate over the buffer size, not terminal_buffer_index
    {
        terminal_buffer[i] = 0;
    }
    terminal_buffer_index = 0; // Reset the index after clearing the buffer
}

// returns true if a command is ready to be processed
bool Terminal::handle_character_input(char input_character)
{
    bool is_enter_recieved = input_character == '\r';
    bool is_backspace_recieved = input_character == 127;
    if (is_enter_recieved)
    {                                                  // Enter key pressed
        terminal_buffer[terminal_buffer_index] = '\0'; // Null-terminate
        printf("\r\n");
        return true;
    }
    if (is_backspace_recieved)
    {
        if (terminal_buffer_index == 0) // if theres nothing to delete
        {
            return false;
        }

        terminal_buffer_index--;                       // Move back one position in the buffer
        terminal_buffer[terminal_buffer_index] = '\0'; // Clear the last character
        printf("%c", input_character);
        return false;
    }
    terminal_buffer[terminal_buffer_index] = input_character;
    terminal_buffer_index++;
    printf("%c", input_character);
    return false;
}

Terminal::Command Terminal::handle_command_input()
{
    if (strcmp(terminal_buffer, "help") == 0)
    {
        return Command::Help;
    }
    else if (strcmp(terminal_buffer, "set_voltage") == 0)
    {
        return Command::SetVoltage;
    }
    else if (strcmp(terminal_buffer, "get_data") == 0)
    {
        return Command::GetData;
    }
    else
    {
        return Command::Unrecognised;
    }
}
