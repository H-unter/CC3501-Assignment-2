#include <cstdlib> // For strtof
#include "terminal.h"

// Terminal constructor
Terminal::Terminal()
{
    stdio_init_all();
    reset_buffer();
    previous_command = "";
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

// Loads the previous command into the terminal buffer
void Terminal::load_previous_command()
{
    if (!previous_command.empty())
    {
        reset_buffer(); // Clear the current buffer
        strncpy(terminal_buffer, previous_command.c_str(), terminal_buffer_size - 1);
        terminal_buffer_index = strlen(terminal_buffer); // Update the buffer index
        printf("\r%s", terminal_buffer);                 // Print the loaded command
    }
}

// Handles character input and checks if a full command has been entered
bool Terminal::handle_character_input(char input_character)
{
    bool is_enter_received = input_character == '\r';
    bool is_backspace_received = input_character == 127;
    bool is_up_arrow_received = input_character == 27;
    bool is_unsupported_character = input_character == '[' || input_character == 'A'; // part of an escape sequence

    if (is_enter_received)
    {
        previous_command = std::string(terminal_buffer); // Store the command as previous_command
        terminal_buffer[terminal_buffer_index] = '\0';   // Null-terminate
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
    if (is_up_arrow_received)
    {
        load_previous_command(); // Load the previous command into the buffer
        return false;
    }
    if (is_unsupported_character)
    {
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
    else if (strcmp(command_buffer, "sdi12_send") == 0)
    {
        return Command::sdi12_send;
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
    cmd.command_name = parse_command(terminal_buffer); // Parse the command from the buffer

    // Handle commands that require numeric input (e.g., set_voltage)
    if (cmd.command_name == Command::set_voltage)
    {
        // Extract the numeric argument (voltage)
        float voltage = parse_float(terminal_buffer); // Assumes voltage is a float after the command
        if (voltage != -1.0f)
        {
            cmd.numeric_argument = voltage; // Set the numeric argument
            printf("Setting voltage to: %.2fV\r\n", voltage);
        }
        else
        {
            cmd.numeric_argument = 0.0f; // Default to 0.0 if no valid voltage is provided
            printf("Error: No valid voltage provided.\r\n");
        }
    }
    // Handle commands that require a string input (e.g., sdi12_send)
    else if (cmd.command_name == Command::sdi12_send)
    {
        // Extract the string argument (SDI-12 command)
        char *sdi12_command = strstr(terminal_buffer, " "); // Find the space after the command
        if (sdi12_command != nullptr)
        {
            cmd.string_argument = std::string(sdi12_command + 1); // Extract and set the string argument
            // printf("Sending SDI-12 command: %s\r\n", cmd.string_argument.c_str());
        }
        else
        {
            cmd.string_argument = ""; // Default to an empty string if no valid SDI-12 command is provided
            printf("Error: No valid SDI-12 command provided.\r\n");
        }
    }
    else
    {
        // For commands that don't use arguments, initialize both arguments to default values
        cmd.numeric_argument = 0.0f;
        cmd.string_argument = "";
    }

    return cmd; // Return the command with its parsed arguments
}
