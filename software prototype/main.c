
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "terminal.h"

/// \tag::uart_advanced[]

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// old code for LEDs
#define CONTENT_OF(addr) (*(volatile uint32_t *)addr)
#define PAD_CONTROL_BASE 0x4001c000
#define REG_PAD_CONTROL_GPIO15 (PAD_CONTROL_BASE + 0x40)
#define CTRL_PDE 0x4
#define CTRL_PUE 0x8

#define thing 15

#define RED_LED 11
#define GREEN_LED 12
#define BLUE_LED 13

// Startup values
#define RED_VALUE 200
#define GREEN_VALUE 0
#define BLUE_VALUE 0

#define PWM_CHAN_B 1
#define PWM_CHAN_A 0

#define MAX_LIGHT 255

volatile uint red_value = RED_VALUE;
volatile uint green_value = GREEN_VALUE;
volatile uint blue_value = BLUE_VALUE;

volatile bool input_ready = false;
volatile char buffer[100];
volatile unsigned int index = 0;
volatile int red_pwm;
volatile int green_pwm;
volatile int blue_pwm;

// RX interrupt handler
void on_uart_rx()
{
  while (uart_is_readable(UART_ID)) // read character
  {
    uint8_t ch = uart_getc(UART_ID); // let "ch" be the character
    buffer[index] = ch;              // save character
    index++;                         // increment the index
    uart_putc(UART_ID, ch);          // print character

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

  // Initialise the GPIOs
  gpio_init(BLUE_LED);
  gpio_init(RED_LED);
  gpio_init(GREEN_LED);

  // Determine the direction (input or output)
  gpio_set_dir(BLUE_LED, true);
  gpio_set_dir(RED_LED, true);
  gpio_set_dir(GREEN_LED, true);

  // PWM stuff
  gpio_set_function(RED_LED, GPIO_FUNC_PWM);
  gpio_set_function(GREEN_LED, GPIO_FUNC_PWM);
  gpio_set_function(BLUE_LED, GPIO_FUNC_PWM);

  uint slice_num_red = pwm_gpio_to_slice_num(RED_LED);
  uint slice_num_green = pwm_gpio_to_slice_num(GREEN_LED);
  uint slice_num_blue = pwm_gpio_to_slice_num(BLUE_LED);

  pwm_set_wrap(slice_num_red, 254);
  pwm_set_wrap(slice_num_green, 254);
  pwm_set_wrap(slice_num_blue, 254);

  pwm_set_enabled(slice_num_red, true);
  pwm_set_enabled(slice_num_green, true);
  pwm_set_enabled(slice_num_blue, true);

  // Handle the various interesting values of ch here...
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID, 2400);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Actually, we want a different speed
  // The call will return the actual baud rate selected, which will be as close as
  // possible to that requested
  int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

  // Set up a RX interrupt
  // We need to set up the handler first
  // Select correct interrupt for the UART we are using
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  // And set up and enable the interrupt handlers
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);

  // Now enable the UART to send interrupts - RX only
  uart_set_irq_enables(UART_ID, true, false);

  term_set_color(35, 30);
  term_cls();         // clear terminal
  term_move_to(1, 6); // set cursor position

  // OK, all set up.
  // Lets send a basic string out, and then run a loop and wait for RX interrupts
  // The handler will count them, but also reflect the incoming data back with a slight change!
  uart_puts(UART_ID, "\nWELCOME\n");                        // present the program
  term_move_to(7, 8);                                       // move cursor to RED PWM ratio spot
  printf("RED PWM ratio: %d / %d", red_pwm, MAX_LIGHT);     // print the current red pwm ratio
  term_move_to(7, 9);                                       // move cursor to GREEN PWM ratio spot
  printf("GREEN PWM ratio: %d / %d", green_pwm, MAX_LIGHT); // print the current green pwm ratio
  term_move_to(7, 10);                                      // move cursor to BLUE PWM ratio spot
  printf("BLUE PWM ratio: %d / %d", blue_pwm, MAX_LIGHT);   // print the current blue pwm ratio
  term_move_to(1, 12);                                      // set cursor to buffer spot

  while (true)
  {
    while (!input_ready)
    {
      __asm("wfi");
    }

    // if RED
    if (sscanf(buffer, "red %d\n", &red_pwm) == 1)
    {
      red_value = red_pwm;                                    // set red LED to specified value
      buffer[0] = '\0';                                       // clear buffer
      term_move_to(7, 8);                                     // move cursor to RED PWM ratio spot
      term_erase_line();                                      // erase what was before
      printf("RED PWM ratio: %d / %d\n", red_pwm, MAX_LIGHT); // print current ratio
    }

    // if GREEN
    if (sscanf(buffer, "green %d\n", &green_pwm) == 1)
    {
      green_value = green_pwm;                                    // set red LED to specified value
      buffer[0] = '\0';                                           // clear buffer
      term_move_to(7, 9);                                         // move cursor to GREEN PWM ratio spot
      term_erase_line();                                          // erase what was before
      printf("GREEN PWM ratio: %d / %d\n", green_pwm, MAX_LIGHT); // print current ratio
    }

    // if BLUE
    if (sscanf(buffer, "blue %d\n", &blue_pwm) == 1)
    {
      blue_value = blue_pwm;                                    // set red LED to specified value
      buffer[0] = '\0';                                         // clear buffer
      term_move_to(7, 10);                                      // move cursor to BLUE PWM ratio spot
      term_erase_line();                                        // erase what was before
      printf("BLUE PWM ratio: %d / %d\n", blue_pwm, MAX_LIGHT); // print current ratio
    }

    input_ready = false; // restart the loop
    index = 0;           // make sure the buffer starts from zero

    // Turning on LEDs
    pwm_set_chan_level(slice_num_red, PWM_CHAN_B, red_value);
    pwm_set_chan_level(slice_num_green, PWM_CHAN_A, green_value);
    pwm_set_chan_level(slice_num_blue, PWM_CHAN_B, blue_value);
    sleep_ms(10);

    term_move_to(1, 12); // set cursor position back to buffer
    term_erase_line();   // erase the stuff in the buffer spot
  }
}

