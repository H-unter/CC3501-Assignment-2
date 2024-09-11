#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#include "hardware/uart.h"
#include "hardware/irq.h"
#include <string>

/// \tag::uart_advanced[]

// sdi-12 uart
#define UART_ID_SENSORS uart1
#define BAUD_RATE_SENSORS 1200

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// Check  The pinout of the Raspberry Pi Pico Rev3 board in datasheet
#define UART_TX_PIN_SENSORS 4
#define UART_RX_PIN_SENSORS 5

#define ENABLE_DRIVE_PIN 3


/*!
* \brief sends a break in order to begin UART communication
*/
void uart_send_break(uart_inst_t *uart) {
    uart_set_break(uart, true); // Set the UART break signal
    uart_set_break(uart, false);// Clear the break signal
    sleep_ms(10);
}

/*!
 * \brief Sends a command over UART for SDI-12 communication
 *
 * This function sends a specified SDI-12 command string to the sensor.
 * It first sends a break signal to wake up the sensors, sends the command,
 * and then switches the GPIO pin to receive the response.
 *
 * \param uart The UART instance (e.g., uart1) used for communication.
 * \param command The SDI-12 command string to send (e.g., "0M!").
 */
void uart_send_command(uart_inst_t *uart, char *command) {
    
    
    // Send the SDI-12 command string (e.g., "0M!")
    while (*command) {
        uart_putc(uart, *command++);  // Send each character one by one
    }

    
    // Delay to allow time for the sensor to process the command
    sleep_ms(15);



    // Wait for the response from the sensor
}




int main()
{
  stdio_init_all();

  // SENSORS ////////////////////////////////////////////////////////////////////////////
  // Handle the various interesting values of ch here...
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID_SENSORS, BAUD_RATE_SENSORS);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN_SENSORS, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN_SENSORS, GPIO_FUNC_UART);

  // Initialise the GPIOs
  gpio_init(ENABLE_DRIVE_PIN);

  // Determine the direction (input or output)
  gpio_set_dir(ENABLE_DRIVE_PIN, true);
  printf("test 1\n");
  while (true)
  {
    // Set GPIO pin high to enable transmitting
    gpio_put(ENABLE_DRIVE_PIN, true);
    uart_send_break(UART_ID_SENSORS); // Send the UART break signal to initiate communication
    uart_send_command(UART_ID_SENSORS, "0M!");
    uart_send_command(UART_ID_SENSORS, "0D0!");
    gpio_put(ENABLE_DRIVE_PIN, false); // Set GPIO pin low to enable receiving mode
    sleep_ms(50); // sleep until the response is recieved, not timing critical as the uart stuff comes through a FIFO buffer

    int buffer_size = 128;  // Internal buffer size for storing received characters
    uint8_t ch;
    char internal_buffer[buffer_size];
    // Poll until there is no more data in the UART FIFO or the buffer is full
    int index = 0;  // Reset index for new data reception
    while (uart_is_readable(UART_ID_SENSORS) && index < buffer_size - 1) {
        ch = uart_getc(UART_ID_SENSORS);// Read one character from the UART receive buffer
        internal_buffer[index++] = ch; // Store the character in the internal buffer
        printf("%c\n", ch); // print the character
        sleep_us(1);
    }

    // Null-terminate the string to make it easier to process
    internal_buffer[index] = '\0';


    // Print the full content of the buffer
    printf("Received %d characters: %s\n", index, internal_buffer);
      
    // Wait before sending the next command
    sleep_ms(400);
}
}
    
