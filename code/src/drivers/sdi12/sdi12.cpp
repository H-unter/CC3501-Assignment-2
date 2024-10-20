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

    sleep_ms(SDI12_MAX_RESPONSE_TIME_MS + 5);                  // Wiggle room for sensors out of spec
    absolute_time_t response_start_time = get_absolute_time(); // Get the current time
    uint8_t character;
    const int sdi12_buffer_size = 128; // Internal buffer size for storing received characters
    char message_buffer[sdi12_buffer_size] = {0};
    int buffer_index = 0; // Index for new data reception
    while (uart_is_readable(uart_instance) && buffer_index < sdi12_buffer_size - 1 && !is_response_timed_out(response_start_time))
    {
        character = uart_getc(uart_instance); // Read one character from the UART receive buffer
        bool is_unimportant_character = (character == '\r' || character == '\n' || character == '\0');
        if (!is_unimportant_character)
        {
            message_buffer[buffer_index++] = character; // Store the character in the internal buffer
            sleep_ms(5);                                // Delay to allow the next character to come through
        }
    }
    message_buffer[buffer_index] = '\0'; // Null-terminate the message string
    return std::string(message_buffer);  // Return the received message as a C++ string
    is_dataline_busy = false;
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
