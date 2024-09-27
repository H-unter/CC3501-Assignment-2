#include "loadcell.h"

// Constructor for LoadCell class
LoadCell::LoadCell(int adc_pin, float alpha, float conversion_factor)
    : adc_pin(adc_pin), alpha(alpha), conversion_factor(conversion_factor), smoothed_voltage(0.0f) {}

// Initialize the ADC for the given pin
void LoadCell::init()
{
    adc_init();
    adc_gpio_init(adc_pin);
    adc_select_input(0); // Assuming input 0 for the ADC
}

// Read the ADC value and convert it to voltage
float LoadCell::read_voltage()
{
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
    return -0.4108181 + 3.2397531 * voltage - 1.5099098 * pow(voltage, 2) + 1.7197607 * pow(voltage, 3) - 0.3855563 * pow(voltage, 4);
}

// Public method to sample the voltage and return the mass
float LoadCell::sample_mass()
{
    float voltage = read_voltage();                // Read new voltage
    float smoothed_volt = smooth_voltage(voltage); // Smooth the voltage
    return calculate_mass(smoothed_volt);          // Calculate and return the mass
}
