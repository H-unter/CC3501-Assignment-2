#ifndef LOADCELL_H
#define LOADCELL_H

#include "hardware/adc.h"
#include <cmath>

class LoadCell
{
public:
    // Constructor
    LoadCell(int adc_pin, float alpha = 0.01f, float conversion_factor = 3.3f / (1 << 12));

    // Method to initialize the ADC
    void init();

    // Method to sample the voltage and return the mass (in kg)
    float sample_mass();

private:
    // Method to read and return the raw ADC voltage
    float read_voltage();

    // Method to update and return the smoothed voltage
    float smooth_voltage(float newest_sample);

    // Method to calculate mass based on smoothed voltage
    float calculate_mass(float voltage);

    int adc_pin;             // ADC pin for the load cell
    float alpha;             // Smoothing factor
    float conversion_factor; // ADC to voltage conversion factor
    float smoothed_voltage;  // Holds the smoothed voltage
};

#endif // LOADCELL_H
