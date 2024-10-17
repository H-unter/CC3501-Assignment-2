#include <cstdlib> // For strtof
#include "terminal.h"

// Terminal constructor
Terminal::Terminal()
{
    stdio_init_all();
    reset_buffer();
}

// Clears the terminal screen
void Terminal::clear_terminal()
{
    printf("\033[2J\033[H");
}

// Resets the input buffer
void Terminal::reset_buffer()
{
    for (int i = 0; i < terminal_buffer_size; i++)
    {
        terminal_buffer[i] = 0;
    }
    terminal_buffer_index = 0;
}

// Handles character input and checks if a full command has been entered
bool Terminal::handle_character_input(char input_character)
{
    bool is_enter_received = input_character == '\r';
    bool is_backspace_received = input_character == 127;

    if (is_enter_received)
    {
        terminal_buffer[terminal_buffer_index] = '\0'; // Null-terminate
        printf("\r\n");
        return true;
    }
    if (is_backspace_received)
    {
        if (terminal_buffer_index == 0)
        {
            return false;
        }
        terminal_buffer_index--;
        terminal_buffer[terminal_buffer_index] = '\0';
        printf("%c", input_character);
        return false;
    }

    terminal_buffer[terminal_buffer_index] = input_character;
    terminal_buffer_index++;
    printf("%c", input_character);
    return false;
}

// Parse the command name from the input string
Terminal::Command::command_name_t Terminal::parse_command(const char *input)
{
    char command_buffer[50];             // Buffer to hold the command word
    sscanf(input, "%s", command_buffer); // Extract the first word (the command)

    if (strcmp(command_buffer, "help") == 0)
    {
        return Command::help;
    }
    else if (strcmp(command_buffer, "set_voltage") == 0)
    {
        return Command::set_voltage; // We handle arguments separately
    }
    else if (strcmp(command_buffer, "get_data") == 0)
    {
        return Command::get_data;
    }
    else if (strcmp(command_buffer, "shutdown") == 0)
    {
        return Command::shutdown;
    }
    else
    {
        return Command::unrecognised;
    }
}

// Parse a floating-point number from the input
float Terminal::parse_float(const char *input)
{
    char *arg = strstr(input, " ");
    if (arg != nullptr)
    {
        return strtof(arg + 1, nullptr); // Now strtof should be recognized
    }
    return -1.0f; // Return -1 if no valid number is found
}

// Handle the command input and extract the command name and argument (if any)
Terminal::Command Terminal::handle_command_input()
{
    Command cmd;
    cmd.command_name = parse_command(terminal_buffer);

    if (cmd.command_name == Command::set_voltage)
    {
        // For set_voltage, extract the argument (voltage)
        float voltage = parse_float(terminal_buffer);
        if (voltage != -1.0f)
        {
            cmd.argument = voltage;
            printf("Setting voltage to: %.2fV\r\n", voltage);
        }
        else
        {
            cmd.argument = 0.0f;
            printf("Error: No valid voltage provided.\r\n");
        }
    }
    else
    {
        cmd.argument = 0.0f; // Default argument for commands without arguments
    }

    return cmd;
}
