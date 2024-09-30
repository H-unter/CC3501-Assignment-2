#include "dac.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <gpio.h>

#define DAC_I2C_INSTANCE i2c0
#define DAC_SDA_PIN 7
#define DAC_SCL_PIN 6
#define DAC_ADDR 0x60  // I2C address of the MCP47X6 (CHECK THIS ADDRESS!!!!)
#define DAC_I2C_BAUD_RATE 100 * 1000 // Standard mode is 100kHz

// Constructor
DAC::DAC() {
  // Initialization can be done here if needed
}

// Initialize DAC and I2C communication
void DAC::init() {
  // Initialize I2C at 100kHz
  i2c_init(DAC_I2C_INSTANCE, DAC_I2C_BAUD_RATE);

  // configure the SDA and SCL pins to be set as GPIO_FUNC_I2C 
  // (so that they are internally multiplexed to the I2C hardware)
  gpio_set_function(DAC_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(DAC_SCL_PIN, GPIO_FUNC_I2C);
}

// Set DAC voltage
void DAC::set_voltage(uint16_t value) {
  if (value > 4095) {
    value = 4095;  // Clamp to max if out of range
  }

  uint8_t data[2];
  data[0] = (value >> 4) & 0xFF;    // High byte (8 MSBs)
  data[1] = (value << 4) & 0xFF;    // Low byte (4 LSBs)

  // Write the data to the DAC via I2C
  i2c_write_blocking(DAC_I2C_INSTANCE, DAC_ADDR, data, 2, false);
}
