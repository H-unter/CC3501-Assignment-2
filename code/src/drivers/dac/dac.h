#ifndef DAC_H
#define DAC_H
#include <stdio.h>
#include <stdint.h>
class DAC
{
public:
    // Constructor
    DAC();

    // Method to initialise DAC
    void init();

    // Method to set DAC voltage
    void set_voltage(float);

    bool dac_write_register(uint8_t, uint8_t);

    void configure_default();
};

#endif // DAC_H