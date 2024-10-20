#include "sdi12.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "board.h"

// Constructor to initialize the UART and GPIO pins
SDI12::SDI12(uart_inst_t *uart_instance)
    : uart_instance(uart_instance), tx_pin(SDI12_UART_TX_PIN), rx_pin(SDI12_UART_RX_PIN), enable_pin(ENABLE_DRIVE_PIN), is_dataline_busy(false)
{
    // Initialise the GPIOs to control whether data line is driven or received from
    gpio_init(enable_pin);
    // Initialize the UART
    uart_init(uart_instance, SDI12_BAUD_RATE);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    uart_set_hw_flow(uart_instance, false, false);          // SDI-12 devices don’t support hardware flow control like CTS/RTS, so it isn't needed.
    uart_set_format(uart_instance, 7, 1, UART_PARITY_EVEN); // SDI-12 format: 7 data bits, even parity, 1 stop bit pg 13 of SDI-12 datasheet

    // Initialize the GPIO for transmission enable
    gpio_init(enable_pin);
    gpio_set_dir(enable_pin, true);
}

void SDI12::send_break()
{
    is_dataline_busy = true;
    gpio_put(enable_pin, true);          // Enable transmitting mode
    uart_set_break(uart_instance, true); // hold the break for an extra ms (uart break in this case happens to be about the same time as the sdi-12 break)
    // sleep_ms(1);                          // Hold the break for at least 1 ms
    uart_set_break(uart_instance, false); // Clear the break signal
    sleep_ms(10);                         // Wait 8.33 ms before starting communication
    is_dataline_busy = false;
}

void SDI12::send_command(const std::string &command, bool start_listening_after_command)
{
    is_dataline_busy = true;
    uart_write_blocking(uart_instance, (const uint8_t *)command.c_str(), command.length());
    uart_tx_wait_blocking(uart_instance); // Wait until the command is fully transmitted
    if (start_listening_after_command)
    {
        set_data_line_driven(false);
    }
}

std::string SDI12::receive_command_blocking()
{
    absolute_time_t response_start_time = get_absolute_time(); // Get the current time

    const int sdi12_buffer_size = 128; // Internal buffer size for storing received characters
    char message_buffer[sdi12_buffer_size] = {0};
    int buffer_index = 0;                  // Index for new data reception
    bool carriage_return_received = false; // Tracks if '\r' has been received

    bool is_end_of_message = false;
    uint8_t ch;

    while (buffer_index < sdi12_buffer_size - 1 && !is_response_timed_out(response_start_time))
    {
        if (!uart_is_readable(uart_instance))
        {
            tight_loop_contents();
            continue;
        }
        ch = uart_getc(uart_instance); // Read one character from the UART receive buffer
        if (ch != '\0')
        {
            message_buffer[buffer_index++] = ch;
        }

        bool is_end_of_message = (ch == '\n' && buffer_index > 1 && message_buffer[buffer_index - 2] == '\r');
        if (is_end_of_message)
        {
            message_buffer[buffer_index - 2] = '\0'; // Null-terminate the message string before the '\r\n' characters
            break;
        }
    }
    message_buffer[buffer_index] = '\0'; // Null-terminate the message string
    is_dataline_busy = false;
    return std::string(message_buffer); // Return the received message as a C++ string
}

bool SDI12::is_response_timed_out(absolute_time_t start_time)
{
    int64_t time_diff_us = absolute_time_diff_us(start_time, get_absolute_time());
    int64_t time_diff_ms = time_diff_us / 1000;
    return time_diff_ms > SDI12_MAX_TRANSMIT_TIME_MS;
}

void SDI12::set_data_line_driven(bool is_driven)
{
    gpio_put(enable_pin, is_driven);
}

int SDI12::parse_wait_time_from_measure_response(const std::string &response)
{
    if (response.length() >= 3)
    {
        // Extract the wait time from characters 2 and 3 (index 1 and 2)
        std::string wait_time_str = response.substr(1, 2);

        // Manually check if both characters are digits
        if (isdigit(wait_time_str[0]) && isdigit(wait_time_str[1]))
        {
            return std::stoi(wait_time_str); // Convert the string to an integer
        }
        else
        {
            printf("Error: Invalid wait time format in response.\n");
            return -1;
        }
    }

    // If the response is too short or malformed, return -1
    printf("Error: Response too short to parse wait time.\n");
    return -1;
}

float SDI12::parse_value_from_response(const std::string &response)
{
    // Find the position of the '+' sign
    size_t pos = response.find('+');
    if (pos == std::string::npos || pos + 1 >= response.size())
    {
        // Handle invalid response format
        return -1.0f; // Return an error value
    }

    // Extract the part of the string after the '+' and convert it to a float
    return std::stof(response.substr(pos + 1));
}