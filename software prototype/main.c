
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "terminal.h"

/// \tag::uart_advanced[]

// debug uart
#define UART_ID_DEBUG uart0
#define BAUD_RATE_DEBUG 115200

// sdi-12 uart
#define UART_ID_SENSORS uart1
#define BAUD_RATE_SENSORS 1200

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN_DEBUG 0
#define UART_RX_PIN_DEBUG 1
// Check  The pinout of the Raspberry Pi Pico Rev3 board in datasheet
#define UART_TX_PIN_SENSORS 4
#define UART_RX_PIN_SENSORS 5

volatile bool input_ready = false;
volatile char buffer[100];
volatile unsigned int index = 0;

// RX interrupt handler
void on_uart_rx_debug()
{
  while (uart_is_readable(UART_ID_DEBUG)) // read character
  {
    uint8_t ch = uart_getc(UART_ID_DEBUG); // let "ch" be the character
    buffer[index] = ch;              // save character
    index++;                         // increment the index
    uart_putc(UART_ID_DEBUG, ch);          // print character

    // backspace feature
    if (ch == 0x7f)
    {
      if ((index == 0) || (index == 1)) // a reminder that "index" is unsigned and cannot go negative
      {
        index = 0; // may zero be furthest it may go
      }
      else
      {
        index = index - 2; // move back 2 because "index" got incremented
      }
    }

    if ((ch == '\r') || (ch == '\n')) // return/new line
    {
      buffer[index] = 0;  // add trailing null
      input_ready = true; // exit loop
    }
  }
}

int main()
{
  stdio_init_all();

  // DEBUG //////////////////////////////////////////////////////////////////////////////////
  // Handle the various interesting values of ch here...
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID_DEBUG, 2400);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN_DEBUG, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN_DEBUG, GPIO_FUNC_UART);

  // Actually, we want a different speed
  // The call will return the actual baud rate selected, which will be as close as
  // possible to that requested
  int __unused actual_debug = uart_set_baudrate(UART_ID_DEBUG, BAUD_RATE_DEBUG);

  // Set up a RX interrupt
  // We need to set up the handler first
  // Select correct interrupt for the UART we are using
  int UART_IRQ_DEBUG = UART_ID_DEBUG == uart0 ? UART0_IRQ : UART1_IRQ;

  // And set up and enable the interrupt handlers
  irq_set_exclusive_handler(UART_IRQ_DEBUG, on_uart_rx_debug);
  irq_set_enabled(UART_IRQ_DEBUG, true);

  // Now enable the UART to send interrupts - RX only
  uart_set_irq_enables(UART_ID_DEBUG, true, false);


  // SENSORS ////////////////////////////////////////////////////////////////////////////
  // Handle the various interesting values of ch here...
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID_SENSORS, 2400);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN_SENSORS, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN_SENSORS, GPIO_FUNC_UART);

  // Actually, we want a different speed
  // The call will return the actual baud rate selected, which will be as close as
  // possible to that requested
  int __unused actual_sensors = uart_set_baudrate(UART_ID_SENSORS, BAUD_RATE_SENSORS);

  // // Set up a RX interrupt
  // // We need to set up the handler first
  // // Select correct interrupt for the UART we are using
  // int UART_IRQ_SENSORS = UART_ID_SENSORS == uart1 ? UART1_IRQ : UART0_IRQ;

  // // And set up and enable the interrupt handlers
  // irq_set_exclusive_handler(UART_IRQ_SENSORS, on_uart_tx);
  // irq_set_enabled(UART_IRQ_SENSORS, true);

  // // Now enable the UART to send interrupts - RX only
  // uart_set_irq_enables(UART_ID_DEBUG, true, false);


  // TERMINAL ///////////////////////////////////////////////////////////////////////////////
  term_set_color(35, 30);
  term_cls();         // clear terminal
  term_move_to(1, 6); // set cursor position

  // OK, all set up.
  // Lets send a basic string out, and then run a loop and wait for RX interrupts
  // The handler will count them, but also reflect the incoming data back with a slight change!
  uart_puts(UART_ID_DEBUG, "\nWELCOME\n");                        // present the program
  term_move_to(7, 8);                                       // move cursor to RED PWM ratio spot
  printf("no break\n");                                     // print the current red pwm ratio
  term_move_to(1, 12);                                      // set cursor to buffer spot

  while (true)
  {
    while (!input_ready)
    {
      __asm("wfi");
    }

    // if RED
    if (sscanf(buffer, "break\n") == 1)
    { 
      uart_set_break(uart1, true);
      sleep_ms(12);
      uart_set_break(uart1, false);                     
      buffer[0] = '\0';                                       // clear buffer
      term_move_to(7, 8);                                     // move cursor to break spot
      term_erase_line();                                      // erase what was before
      printf("break sent\n"); // print current ratio
    }
    
    input_ready = false; // restart the loop
    index = 0;           // make sure the buffer starts from zero

    term_move_to(1, 12); // set cursor position back to buffer
    term_erase_line();   // erase the stuff in the buffer spot
  }
}

