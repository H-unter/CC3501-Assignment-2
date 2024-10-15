#ifndef DAC_H
#define DAC_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class DAC
{
public:
    enum vref {
        VDD,
        UNBUFFERED,
        BUFFERED
    };

    enum power_down {
        NORMAL, // PD_00
        PD_01,
        PD_10,
        PD_11
    };

    enum gain {
        ONE,
        TWO
    };



    DAC();                                          // Constructor
    void init();                                    // Initializes the DAC and I2C communication
    void set_voltage(float voltage);                // Sets the DAC output voltage
    bool write_all_memory(uint8_t settings, uint16_t dac_value); // Writes to a DAC register
    uint8_t set_vref(vref vref_setting);
    uint8_t set_power_down(power_down power_down_setting);
    uint8_t set_gain(gain gain_setting);

private:
    uint8_t i2c_addr; // I2C Address for the MCP4716A0T-E/CH (0x60)
    uint8_t vref_value;
    uint8_t power_down_value;
    uint8_t gain_value;
};

#endif // DAC_H
