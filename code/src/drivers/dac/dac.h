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
    void set_voltage(uint16_t);
};

#endif // DAC_H