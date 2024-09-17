#include "sdi12.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "board.h"

// Constructor to initialize the UART and GPIO pins
SDI12::SDI12(uart_inst_t *uart_instance)
    : uart_instance(uart_instance), tx_pin(SDI12_UART_TX_PIN), rx_pin(SDI12_UART_RX_PIN), enable_pin(ENABLE_DRIVE_PIN)
{
    // Initialise the GPIOs to control whether data line is driven or received from
    gpio_init(enable_pin);
    // Initialize the UART
    uart_init(uart_instance, enable_pin);
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
    gpio_put(enable_pin, true);          // Enable transmitting mode
    uart_set_break(uart_instance, true); // hold the break for an extra ms (uart break in this case happens to be about the same time as the sdi-12 break)
    // sleep_ms(1);                          // Hold the break for at least 1 ms
    uart_set_break(uart_instance, false); // Clear the break signal
    sleep_ms(10);                         // Wait 8.33 ms before starting communication
}

void SDI12::send_command(const std::string &command, bool start_listening_after_command)
{
    uart_write_blocking(uart_instance, (const uint8_t *)command.c_str(), command.length());
    uart_tx_wait_blocking(uart_instance); // Wait until the command is fully transmitted
    if (start_listening_after_command)
    {
        set_data_line_driven(false);
    }
}

bool SDI12::is_timed_out(absolute_time_t start_time)
{
    int64_t time_diff_us = absolute_time_diff_us(start_time, get_absolute_time());
    return (time_diff_us / 1000) > SDI12_MAX_RESPONSE_TIME_MS;
}

void SDI12::set_data_line_driven(bool is_driven)
{
    gpio_put(enable_pin, is_driven);
}
