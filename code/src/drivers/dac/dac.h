#ifndef DAC_H
#define DAC_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class DAC
{
public:
    /*!
     * \brief Enum representing the reference voltage options for the DAC
     */
    enum vref {
        VDD,        /*!< Use VDD as reference */
        UNBUFFERED, /*!< Use Vref pin (unbuffered) */
        BUFFERED    /*!< Use Vref pin (buffered) */
    };

    /*!
     * \brief Enum representing power down modes for the DAC
     */
    enum power_down {
        NORMAL, /*!< Not Powered Down (Normal operation) */
        PD_01,  /*!< Powered Down – Vout is loaded with 1 kΩ resistor to ground */
        PD_10,  /*!< Powered Down – Vout is loaded with 100 kΩ resistor to ground */
        PD_11   /*!< Powered Down – Vout is loaded with 500 kΩ resistor to ground */
    };

    /*!
     * \brief Enum representing gain options for the DAC
     */
    enum gain {
        ONE, /*!< Gain of 1x */
        TWO  /*!< Gain of 2x */
    };

    /*!
     * \brief DAC Constructor
     */
    DAC();

    /*!
     * \brief Initialize the DAC and set up I2C communication
     */
    void init();

    /*!
     * \brief Set the output voltage of the DAC
     * \param voltage The desired output voltage
     */
    void set_voltage(float voltage);

    /*!
     * \brief Write settings and DAC value to all memory registers
     * \param settings 8-bit configuration settings
     * \param dac_value 16-bit DAC value to set the output voltage
     * \return true if write is successful, false otherwise
     */
    bool write_all_memory(uint8_t settings, uint16_t dac_value);

    /*!
     * \brief Set the DAC reference voltage
     * \param vref_setting The desired reference voltage (Vref)
     * \return Updated reference voltage setting (as uint8_t)
     */
    uint8_t set_vref(vref vref_setting);

    /*!
     * \brief Set the DAC power down mode
     * \param power_down_setting The desired power down mode
     * \return Updated power down mode setting (as uint8_t)
     */
    uint8_t set_power_down(power_down power_down_setting);

    /*!
     * \brief Set the DAC gain
     * \param gain_setting The desired gain setting
     * \return Updated gain setting (as uint8_t)
     */
    uint8_t set_gain(gain gain_setting);

private:
    uint8_t vref_value;        /*!< Current Vref setting */
    uint8_t power_down_value;  /*!< Current power down setting */
    uint8_t gain_value;        /*!< Current gain setting */
};

#endif // DAC_H
