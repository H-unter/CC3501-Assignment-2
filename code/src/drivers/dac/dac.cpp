#include "dac.h"

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "drivers/logging/logging.h"

#define DAC_I2C_INSTANCE i2c0
#define DAC_SDA_PIN 7
#define DAC_SCL_PIN 6
#define DAC_ADDR 0x60                // I2C address of the MCP4716A0 (10 bits)
#define DAC_I2C_BAUD_RATE 100 * 1000 // Standard mode is 100kHz

// Constructor
DAC::DAC()
{
  // Initialization can be done here if needed
}

// Initialize DAC and I2C communication
void DAC::init()
{
  // Initialize I2C at 100kHz
  i2c_init(DAC_I2C_INSTANCE, DAC_I2C_BAUD_RATE);

  // Set SDA and SCL pins as I2C
  gpio_set_function(DAC_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(DAC_SCL_PIN, GPIO_FUNC_I2C);

  // Configure DAC to default settings
  configure_default();
}

void DAC::configure_default() {
  // Set configuration bits to default values
  // Example: Setting VREF to VDD, Gain to 1, Power down to normal operation
  uint8_t config_byte = 0b00000000; // Set bits according to the desired default configuration
  
  // Writing configuration to volatile memory (adjust this if needed)
  dac_write_register(0x00, config_byte); // Assuming register 0x00 is for config
}

void DAC::set_voltage(float voltage) {
  // Clamp voltage to the 0-5V range
  if (voltage < 0.0f) {
    voltage = 0.0f;
  } else if (voltage > 5.0f) {
    voltage = 5.0f;
  }

  // Scale voltage to the DAC range (0-4095)
  uint16_t value = (uint16_t)((voltage / 5.0f) * 4095.0f);

  // Write the scaled value to the volatile DAC register
  uint8_t data[3];
  data[0] = 0b01000000;             // Control byte (010: normal operation)
  data[1] = (value >> 4) & 0xFF;    // High byte (8 MSBs)
  data[2] = (value << 4) & 0xFF;    // Low byte (4 LSBs)

  // Write the data to the DAC via I2C
  i2c_write_blocking(DAC_I2C_INSTANCE, DAC_ADDR, data, 3, false);
}

bool DAC::dac_write_register(uint8_t reg, uint8_t data) {
  uint8_t buf[2];
  buf[0] = reg;
  buf[1] = data;
  int bytes_written = i2c_write_blocking(DAC_I2C_INSTANCE, DAC_ADDR, buf, 2, false);
  if (bytes_written != 2) {
    log(LogLevel::ERROR, "Failed to write to DAC register.");
    return false;
  }
  return true;
}
