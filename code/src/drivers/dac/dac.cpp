#include "dac.h"

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "drivers/logging/logging.h"

#define DAC_I2C_INSTANCE i2c0
#define DAC_SDA_PIN 7
#define DAC_SCL_PIN 6
#define DAC_ADDR 0b11000000          // I2C address of the MCP4716A0 (10 bits)
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
  // 1 0 0 VREF1 VREF0 PD1 PD0 G
  //                          \_/
  //                   \_____/Gain      => 1x
  //        \_________/ Power down bits => Normal operation
  // \___/  Reference voltage           => Vdd (unbuffered)
  // Command bits                       => Write Volatile Configuration Bits
  //
  // Refer to Table 4-4 (page 42) and Section 6.4 (page 53) of datasheet for more detail
  // Set everything to zero
  // => 0b10000000
  uint8_t data[1];
  data[0] = 0b10000000;

  // Write the data to the DAC via I2C
  i2c_write_blocking(DAC_I2C_INSTANCE, DAC_ADDR, data, 1, false);
}

void DAC::set_voltage(float voltage) {
  // Clamp voltage to the 0-5V range
  if (voltage < 0.0f) {
    voltage = 0.0f;
  } else if (voltage > 5.0f) {
    voltage = 5.0f;
  }

  // Scale voltage to the DAC range (0-1023 for a 10-bit DAC)
  uint16_t value = (uint16_t)((voltage / 5.0f) * 1023.0f);

  // Prepare the command and data bytes
  // Match this format: 0 0 0 0 D09 D08 D07 D06 D05 D04 D03 D02 D01 D00 X X
  uint8_t data[2];
  data[0] = (value >> 6) & 0x3F;  // Extract D09 to D04 and mask lower bits
  data[1] = (value << 2) & 0xFC;  // Extract D03 to D00, and shift left for X X bits

  // Write the data to the DAC via I2C
  i2c_write_blocking(DAC_I2C_INSTANCE, DAC_ADDR, data, 2, false);
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
