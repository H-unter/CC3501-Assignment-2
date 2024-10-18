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
    int test_number = 0;

    while (true)
    {
        printf("Starting SDI-12 test %d\n", test_number);
        // Set GPIO pin high to enable transmitting
        sdi12.set_data_line_driven(true);
        sdi12.send_break(); // Send the break signal to initiate communication
        sdi12.send_command("0I!", true);
        absolute_time_t start_time = get_absolute_time();
        while (!uart_is_readable(SDI12_UART_INSTANCE) && !sdi12.is_timed_out(start_time))
        { // wait for the start of the repsonse
            tight_loop_contents();
        }
        int buffer_size = 128;
        uint8_t ch;
        char message_buffer[buffer_size] = {0};
        int buffer_index = 0; // Reset index for new data reception
        while (uart_is_readable(SDI12_UART_INSTANCE) && buffer_index < buffer_size - 1)
        {
            ch = uart_getc(SDI12_UART_INSTANCE); // Read one character from the UART receive buffer
            if (ch != '\0')
            {
                message_buffer[buffer_index++] = ch; // Store the character in the internal buffer
                sleep_ms(15);                        // enough of a delay to allow the next character to come through
            }
        }
        message_buffer[buffer_index] = '\0'; // Null-terminate the string
        printf("Received %d characters; message: %s\n", buffer_index, message_buffer);

        // Wait before sending the next command
        sleep_ms(1000);
        test_number++;
    }
}
