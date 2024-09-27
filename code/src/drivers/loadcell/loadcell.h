#ifndef LOADCELL_H
#define LOADCELL_H

#include "hardware/adc.h"
#include <cmath>

/*!
 * \brief A class to interface with a load cell and calculate mass using ADC.
 */
class LoadCell
{
public:
    /*!
     * \brief Constructor for the LoadCell class
     * The constructor sets default values for the internal variables but does not initialize the ADC or the smoothing factor. Use `init()` to initialize the load cell.
     */
    LoadCell();

    /*!
     * \brief Initialize the LoadCell class, ADC pin, and smoothing factor
     * This method initializes the ADC, sets the appropriate ADC pin for the load cell, and configures the smoothing factor for voltage.
     * \param adc_pin The ADC pin connected to the load cell
     * \param alpha The smoothing factor for voltage (default is 0.01)
     */
    void init(int adc_pin, float alpha = 0.01f);

    /*!
     * \brief Sample the voltage from the load cell and calculate the mass
     * This method reads the ADC, smooths the voltage using exponential moving average, and calculates the mass based on a polynomial model.
     * \return The mass in kilograms as a floating-point number
     */
    float sample_mass();

private:
    /*!
     * \brief Read the ADC value and convert it to a voltage
     * This method reads the raw ADC value and converts it to a voltage using the specified conversion factor.
     * \return The voltage in volts as a floating-point number
     */
    float read_voltage();

    /*!
     * \brief Update the smoothed voltage using the exponential moving average
     * \param newest_sample The most recent voltage sample to include in the smoothing calculation
     * \return The updated smoothed voltage as a floating-point number
     */
    float smooth_voltage(float newest_sample);

    /*!
     * \brief Calculate the mass based on the smoothed voltage
     * The mass is calculated using a 4th-degree polynomial model derived from regression analysis.
     * \param voltage The smoothed voltage value
     * \return The calculated mass in kilograms as a floating-point number
     */
    float calculate_mass(float voltage);

    int adc_pin;                                /*!< The ADC pin connected to the load cell */
    float alpha;                                /*!< The smoothing factor for the voltage */
    float conversion_factor = 3.3f / (1 << 12); /*!< The factor to convert ADC readings to voltage */
    float smoothed_voltage = 0.0f;              /*!< The current smoothed voltage value */
};

#endif // LOADCELL_H
