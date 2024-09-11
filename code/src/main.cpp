#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#include "hardware/uart.h"
#include "hardware/irq.h"
#include <string>

// hardware related definitions
#define UART_INSTANCE uart1
#define SDI12_UART_TX_PIN 4
#define SDI12_UART_RX_PIN 5
#define ENABLE_DRIVE_PIN 3

// sdi-12 parameter definitions
#define SDI12_BAUD_RATE 1200
// sensor response/transmit times (pg.41 of datasheet)
#define SDI12_MAX_RESPONSE_TIME_MS 15
#define SDI12_MAX_TRANSMIT_TIME_MS 380            // min sensor response time for most commands
#define SDI12_MAX_TRANSMIT_TIME_CONCURRENT_MS 810 // max sensor response time for concurrent measurement

/*!
 * \brief sends a break in order to begin UART communication
 */
void uart_send_break(uart_inst_t *uart)
{
    uart_set_break(uart, true);  // Set the UART break signal
    sleep_ms(1);                 // hold the break for an extra ms (uart break in this case happens to be about the same time as the sdi-12 break)
    uart_set_break(uart, false); // Clear the break signal
    sleep_ms(10);                // Wait at least 8.33 ms to begin communication
}

/*!
 * \brief Sends a command over UART for SDI-12 communication.
 *
 * This function sends a specified SDI-12 command string to the sensor.
 * It will wait until the command is fully transmitted before returning.
 *
 * \param uart The UART instance (e.g., uart1) used for communication.
 * \param command The SDI-12 command as a C++ string to send.
 */
void uart_send_command(uart_inst_t *uart, const std::string &command)
{
    // Convert the C++ string to a C-style string for UART transmission
    uart_write_blocking(uart, (const uint8_t *)command.c_str(), command.length());
    uart_tx_wait_blocking(uart); // Wait until the transmission is complete
}

/*!
 * \brief Checks if the current time has exceeded the global timeout value.
 *
 * \param start_time The time when the operation started (in absolute time format).
 * \return true if the timeout has been exceeded, false otherwise.
 */
bool is_timed_out(absolute_time_t start_time)
{
    // Calculate the time difference in microseconds
    int64_t time_diff_us = absolute_time_diff_us(start_time, get_absolute_time());

    // Convert the time difference to milliseconds and compare with the global timeout
    return (time_diff_us / 1000) > SDI12_MAX_RESPONSE_TIME_MS;
}

int main()
{
    stdio_init_all();
    uart_init(UART_INSTANCE, SDI12_BAUD_RATE);
    gpio_set_function(SDI12_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(SDI12_UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_INSTANCE, false, false);          // SDI-12 devices don’t support hardware flow control like CTS/RTS, so it isn't needed.
    uart_set_format(UART_INSTANCE, 7, 1, UART_PARITY_EVEN); // SDI-12 format: 7 data bits, even parity, 1 stop bit pg 13 of SDI-12 datasheet

    // Initialise the GPIOs
    gpio_init(ENABLE_DRIVE_PIN);

    // Determine the direction (input or output)
    gpio_set_dir(ENABLE_DRIVE_PIN, true);
    printf("test 1\n");
    while (true)
    {
        // Set GPIO pin high to enable transmitting
        gpio_put(ENABLE_DRIVE_PIN, true);
        uart_send_break(UART_INSTANCE); // Send the UART break signal to initiate communication
        uart_send_command(UART_INSTANCE, "0I!");
        gpio_put(ENABLE_DRIVE_PIN, false); // Set GPIO pin low to enable receiving mode

        absolute_time_t start_time = get_absolute_time();
        while (!uart_is_readable(UART_INSTANCE) && !is_timed_out(start_time))
        { // wait for the start of the repsonse
            tight_loop_contents();
        }

        // if we make it here, that means we have received the first character of the response

        int buffer_size = 128; // Internal buffer size for storing received characters
        uint8_t ch;
        char message_buffer[buffer_size] = {0};
        // Poll until there is no more data in the UART FIFO or the buffer is full
        int buffer_index = 0; // Reset index for new data reception
        while (uart_is_readable(UART_INSTANCE) && buffer_index < buffer_size - 1)
        {
            ch = uart_getc(UART_INSTANCE);       // Read one character from the UART receive buffer
            message_buffer[buffer_index++] = ch; // Store the character in the internal buffer
            sleep_ms(10);                        // enough of a delay to allow the next character to come through
        }

        // Null-terminate the string to make it easier to process
        message_buffer[buffer_index] = '\0';

        // Print the full content of the buffer
        printf("Received %d characters: %s\n", buffer_index, message_buffer);

        // Wait before sending the next command
        sleep_ms(1000);
    }
}
