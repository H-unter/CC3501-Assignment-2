#ifndef SDI12_H
#define SDI12_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string>

class SDI12
{
public:
    bool is_dataline_busy = false; // Indicates whether the data line is currently busy

    // Constructor: Initializes UART and GPIO pins for SDI-12 communication
    SDI12(uart_inst_t *uart_instance);

    /*!
     * \brief sends a break in order to begin UART communication
     *
     *  sets the enable pin high to enable transmitting mode before sending the break signal
     */
    void send_break();

    /*!
     * \brief Sends a command over UART for SDI-12 communication.
     *
     * This function sends a specified SDI-12 command string to the sensor.
     * It will wait until the command is fully transmitted before returning.
     *
     * \param command The SDI-12 command as a C++ string to send.
     * \param start_listening_after_command If true, the UART will be set to listen for a response after the command is sent. (stop driving the data line)
     */
    void send_command(const std::string &command, bool start_listening_after_command);

    /*!
     * \brief Checks if the current elapsed time has exceeded the maximum response transmit time
     *
     * \param start_time The time when the operation started (in absolute time format). `absolute_time_t start_time = get_absolute_time();`
     * \return true if the timeout has been exceeded, false otherwise.
     */
    bool is_response_timed_out(absolute_time_t start_time);

    /*!
     * \brief Sets the data line to be driven or received from.
     *
     * \param is_driven true to drive the data line, false to receive data.
     */
    void set_data_line_driven(bool is_driven);

    /*!
     * \brief Receives a command from the UART.
     *
     * This function will block until a full command is received from the UART.
     * It will return the received command as a C++ string.
     *
     * \return The received command as a C++ string.
     */
    std::string receive_command_blocking();

    /*!
     * \brief Parses the wait time from a "0M!" response string.
     *
     * \param response The response string from the "0M!" command.
     * \return The wait time in seconds as an integer. Returns -1 if parsing fails.
     */
    int parse_wait_time_from_measure_response(const std::string &response);

    /*!
     * \brief Parses the float value from an SDI-12 response.
     *
     * This function parses the float value after the address and the '+' or '-' sign in the response.
     * Assumes the response format is something like "00+24.1" or "00-12.3".
     *
     * \param response The SDI-12 response string (e.g., "00+24.1").
     * \return The parsed float value. Returns 0.0 if the parsing fails.
     */
    float parse_value_from_response(const std::string &response);

private:
    uart_inst_t *uart_instance; // UART instance (e.g., uart1)
    uint tx_pin;                // UART TX pin
    uint rx_pin;                // UART RX pin
    uint enable_pin;            // GPIO pin to enable transmission
};

#endif // SDI12_H
