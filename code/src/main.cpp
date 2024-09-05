#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#include "hardware/uart.h"
#include "hardware/irq.h"

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

#define ENABLE_PIN 3

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
  gpio_init(ENABLE_PIN);

  // Determine the direction (input or output)
  gpio_set_dir(ENABLE_PIN, true);

  while (true)
  {
    // set to high to enable transmitting
    gpio_put(ENABLE_PIN, true);

    uart_set_break(uart1, true);
    uart_set_break(uart1, false);

    sleep_ms(10);
    uart_putc(UART_ID_SENSORS, '?');
    uart_putc(UART_ID_SENSORS, '!');
    
    sleep_ms(30);
    // set to low to enable receiving
    gpio_put(ENABLE_PIN, false);
    sleep_ms(400); // allow time to receive response from sensor
    
    
}
}
    
