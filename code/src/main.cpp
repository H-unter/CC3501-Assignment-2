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

#define ENABLE_DRIVE_PIN 3

volatile unsigned int index = 0;
volatile char buffer[100];

// Function to print binary representation of a byte
void print_binary(uint8_t value) {
    for (int i = 7; i >= 0; i--) {
        putchar((value & (1 << i)) ? '1' : '0');
    }
}

int main()
{
  stdio_init_all();

  absolute_time_t start_time = get_absolute_time();  // can't equal to zero, so make it equal to real time
  absolute_time_t finish_time = get_absolute_time();  // can't equal to zero, so make it equal to real time
  int64_t receiving_time = 0;

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
    // set to high to enable transmitting
    gpio_put(ENABLE_DRIVE_PIN, true);

    uart_set_break(uart1, true);
    uart_set_break(uart1, false);

    sleep_ms(10); // marking > 8.33ms
    uart_putc(UART_ID_SENSORS, '0');
    uart_putc(UART_ID_SENSORS, 'M');
    uart_putc(UART_ID_SENSORS, '!');

    sleep_ms(15);

    uart_putc(UART_ID_SENSORS, '0');
    uart_putc(UART_ID_SENSORS, 'D');
    uart_putc(UART_ID_SENSORS, '0');
    uart_putc(UART_ID_SENSORS, '!');


    // uart_putc(UART_ID_SENSORS, '?');
    // uart_putc(UART_ID_SENSORS, '!');

    sleep_ms(15);
    // set to low to enable receiving
    gpio_put(ENABLE_DRIVE_PIN, false);

    sleep_ms(50);


      uint8_t ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

       ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

      ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);


            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);


            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);

            ch = uart_getc(UART_ID_SENSORS); // let "ch" be the character
      buffer[index] = ch;              // save character
      index++;                         // increment the index
      printf("%c:\n", ch);



    buffer[index] = 0;                 // add trailing null
    printf("%s\n", buffer);            // print buffer (message from sensor)
    buffer[0] = '\0';                  // clear buffer
    printf("test 2\n");
    index = 0;

  sleep_ms(400); // allow time to receive response from sensor

}
}
    
