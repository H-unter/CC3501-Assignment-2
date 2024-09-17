#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "board.h"      // contains constants for the board
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/sdi12/sdi12.h"

#include <string>

int main()
{
    stdio_init_all();
    SDI12 sdi12(SDI12_UART_INSTANCE);
    printf("test 1\n");
    while (true)
    {
        // Set GPIO pin high to enable transmitting
        sdi12.set_data_line_driven(true);
        sdi12.send_break(); // Send the break signal to initiate communication
        sdi12.send_command("0I!", true);

        absolute_time_t start_time = get_absolute_time();
        while (!uart_is_readable(SDI12_UART_INSTANCE) && !sdi12.is_timed_out(start_time))
        { // wait for the start of the repsonse
            tight_loop_contents();
        }

        // if we make it here, that means we have received the first character of the response

        int buffer_size = 128; // Internal buffer size for storing received characters
        uint8_t ch;
        char message_buffer[buffer_size] = {0};
        // Poll until there is no more data in the UART FIFO or the buffer is full
        int buffer_index = 0; // Reset index for new data reception
        while (uart_is_readable(SDI12_UART_INSTANCE) && buffer_index < buffer_size - 1)
        {
            ch = uart_getc(SDI12_UART_INSTANCE); // Read one character from the UART receive buffer
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
