#ifndef DAC_H
#define DAC_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class DAC
{
public:
    DAC();                                          // Constructor
    void init();                                    // Initializes the DAC and I2C communication
    void set_voltage(float voltage);                // Sets the DAC output voltage
    bool write_register(uint8_t reg, uint8_t data); // Writes to a DAC register

private:
    uint8_t i2c_addr; // I2C Address for the MCP4716A0T-E/CH (0x60)
};

#endif // DAC_H
