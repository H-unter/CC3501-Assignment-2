#include "MCP4716.h"
#include <stdio.h>
#include "board.h"

#define WRITE_ALL_MEMORY_COMMAND 0b01100000

// DAC constructor
MCP4716::MCP4716()
{
}

// Initialize the DAC
void MCP4716::init()
{
  // Initialize I2C
  i2c_init(DAC_I2C_INSTANCE, DAC_I2C_BAUD_RATE); // Based on I2C communication details from page 44
  gpio_set_function(DAC_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(DAC_SCL_PIN, GPIO_FUNC_I2C);
}

// Set Vref bits
uint8_t MCP4716::set_vref(vref vref_setting)
{
  switch (vref_setting)
  {
  case VDD:
    vref_value = 0b00000000;
    break;
  case UNBUFFERED:
    vref_value = 0b00010000;
    break;
  case BUFFERED:
    vref_value = 0b00011000;
    break;
  }
  return 0;
}

// Set Power Down bits
uint8_t MCP4716::set_power_down(power_down power_down_setting)
{
  switch (power_down_setting)
  {
  case NORMAL:
    power_down_value = 0b00000000;
    break;
  case PD_01:
    power_down_value = 0b00000010;
    break;
  case PD_10:
    power_down_value = 0b00000100;
    break;
  case PD_11:
    power_down_value = 0b00000110;
    break;
  }
  return 0;
}

// Set Gain
uint8_t MCP4716::set_gain(gain gain_setting)
{
  switch (gain_setting)
  {
  case ONE:
    gain_value = 0b00000000;
    break;
  case TWO:
    gain_value = 0b00000001;
    break;
  }
  return 0;
}

// Write to DAC register
bool MCP4716::write_all_memory(uint8_t settings, uint16_t dac_value)
{
  // Prepare the settings and data bytes:
  // C2 C1 C0 VREF1 VREF0 PD1 PD0 G D09 D08 D07 D06 D05 D04 D03 D02 D01 D00 X X X X X X
  //                                                                        \_________/
  //                                \_____________________________________/  Don't cares => dac_value
  //                             \_/                Data bits                            => dac_value
  //                      \_____/ Gain                                                   => settings
  //          \__________/ Power down bits                                               => settings
  // \_______/ Reference voltage                                                         => settings
  // Command bits                                                                        => settings
  //

  uint8_t com = settings;         
  uint16_t set = dac_value << 6;
  uint8_t set_upper = (set >> 8); 
  uint8_t set_lower = set & 0xFF; 

  uint8_t buff[3];
  buff[0] = com;
  buff[1] = set_upper;
  buff[2] = set_lower;

  int result = i2c_write_blocking(DAC_I2C_INSTANCE, DAC_I2C_ADDRESS, buff, 3, false);
  if (result < 0)
  {
    printf("[DAC] Failed to write to DAC.\n");
    return false;
  }
  return true;
}

// Set the DAC output voltage (0V - 5V)
void MCP4716::set_voltage(float voltage)
{
  // Clamp the voltage to 0-5V range (based on MCP4716 voltage limits)
  if (voltage < 0.0f)
    voltage = 0.0f;
  if (voltage > 5.0f)
    voltage = 5.0f;

  // Convert voltage to 10 bit binary
  uint16_t dac_value = (uint16_t)((voltage / 5.0f) * 1023.0f);

  // Define settings
  uint8_t settings;
  uint8_t command_value = WRITE_ALL_MEMORY_COMMAND;
  settings = command_value + vref_value + power_down_value + gain_value;

  // Write to all memory
  write_all_memory(settings, dac_value);
}
