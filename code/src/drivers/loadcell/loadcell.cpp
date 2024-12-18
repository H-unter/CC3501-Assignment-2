#include "loadcell.h"

// Constructor for LoadCell class
LoadCell::LoadCell() : adc_pin(0), alpha(0.01f) {}

/*!
 * \brief Initialize the LoadCell class, ADC pin, and smoothing factor
 * This method initializes the ADC, sets the appropriate ADC pin for the load cell, and configures the smoothing factor for voltage.
 * \param adc_pin The ADC pin connected to the load cell
 * \param alpha The smoothing factor for voltage (default is 0.01)
 */
void LoadCell::init(int adc_pin, float alpha)
{
    this->adc_pin = adc_pin;
    this->alpha = alpha;
    // TODO: make alpha propotional to sampling rate

    adc_init();
    adc_gpio_init(adc_pin);
    adc_select_input(adc_pin - 26); // eg adc_pin 26 is adc channel 0
    smoothed_voltage = read_voltage();
}

// Read the ADC value and convert it to voltage
float LoadCell::read_voltage()
{
    adc_select_input(adc_pin - 26); // eg adc_pin 26 is adc channel 0
    return adc_read() * conversion_factor;
}

// Smooth the voltage using exponential moving average
float LoadCell::smooth_voltage(float newest_sample)
{
    smoothed_voltage = alpha * newest_sample + (1.0f - alpha) * smoothed_voltage;
    return smoothed_voltage;
}

// Calculate the mass using the polynomial model
float LoadCell::calculate_mass(float voltage)
{
    // Polynomial model from R
    float mass_kg = -0.4108181 + 3.2397531 * voltage - 1.5099098 * pow(voltage, 2) + 1.7197607 * pow(voltage, 3) - 0.3855563 * pow(voltage, 4);
    float threshold = 0.1f;
    bool is_close_to_zero = (mass_kg < threshold && mass_kg > -threshold);
    if (is_close_to_zero)
    {
        mass_kg = 0.0f;
    }
    return mass_kg;
}

// Public method to sample the voltage and return the mass
float LoadCell::sample_mass()
{
    adc_select_input(adc_pin - 26);
    float voltage = read_voltage();                   // Read new voltage
    float smoothed_voltage = smooth_voltage(voltage); // Smooth the voltage
    // return calculate_mass(smoothed_voltage);          // Calculate and return the mass
    return calculate_mass(voltage); // Calculate and return the mass
}
