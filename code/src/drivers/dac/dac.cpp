#include "dac.h"
#include <stdio.h>
#include "board.h"

// DAC constructor
DAC::DAC() : i2c_addr(DAC_ADDR) {}

// Initialize the DAC
void DAC::init()
{
  printf("[DAC] Initializing DAC...\n");

  // Initialize I2C
  i2c_init(DAC_I2C_INSTANCE, I2C_BAUD_RATE); // Based on I2C communication details from page 44
  gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

  // Ensure pull-ups are set correctly
  printf("[DAC] I2C initialized (SDA: %d, SCL: %d).\n", SDA_PIN, SCL_PIN);
}

// Write to DAC register
bool DAC::write_register(uint8_t reg, uint8_t data)
{
  // Prepare a buffer of two bytes for the register and data
  uint8_t buffer[2] = {reg, data}; // Based on I2C communication and data structure (Page 49)

  // Write the data to the DAC via I2C
  int result = i2c_write_blocking(DAC_I2C_INSTANCE, i2c_addr, buffer, 2, false); // Page 49 mentions sending a command followed by the data
  if (result < 0)
  {
    printf("[DAC] Failed to write to DAC.\n");
    return false;
  }
  return true;
}

// Set the DAC output voltage (0V - 5V)
void DAC::set_voltage(float voltage)
{

   uint8_t command;
  command=96; //Command Bit
    command=command+0; //Vdd


  command=command+00; //Power down Bits

  uint8_t com=(uint8_t) command;
  uint16_t Vout=512;
  uint16_t set= Vout << 6;
  uint8_t set_upper= (uint8_t) (set >> 8);
  uint8_t set_l = (uint8_t) set;
  uint8_t set_lower= (uint8_t) set_l;

  uint8_t buff[3];

  buff[0]=com;
  buff[1]=set_upper;
  buff[2]=set_lower;



int result = i2c_write_blocking(DAC_I2C_INSTANCE, i2c_addr, buff, 3, false); // Page 49 mentions sending a command followed by the data
 
  // // Clamp the voltage to 0-5V range (based on MCP4716 voltage limits)
  // if (voltage < 0.0f)
  //   voltage = 0.0f;
  // if (voltage > 5.0f)
  //   voltage = 5.0f;

  // // Convert voltage to a 10-bit value (0-1023) (based on the 10-bit DAC description on page 49)
  // uint16_t dac_value = (uint16_t)((voltage / 5.0f) * 1023.0f);

  // // To change the voltage of the MCP4716 DAC, you need to send two bytes:
  // // 1. First byte (command + upper 6 bits of the 10-bit DAC value):
  // //    - Bits D9-D4: Upper 6 bits of the 10-bit DAC value (D9 is MSB).
  // //    - Bits PD1-PD0: Power-down control bits, set to 00 for normal operation.
  // // 2. Second byte (lower 4 bits of the DAC value):
  // //    - Bits D3-D0: Lower 4 bits of the 10-bit DAC value (D0 is LSB).
  // //    - Remaining bits are padded with 0s.
  // //
  // // Datasheet reference: Page 49, Figure 6-1 (Data Structure for Writing to DAC Register).
  // uint8_t upper_byte = (dac_value >> 4) & 0xFF; // Upper 8 bits (MSB, based on page 49)
  // uint8_t lower_byte = (dac_value << 4) & 0xF0; // Lower 4 bits (LSB padded with zeros, page 49)

  // // Write the two bytes to the DAC
  // printf("[DAC] Setting DAC output voltage to %.2fV...\n", voltage);
  // printf("[DAC] Scaled voltage to 10-bit DAC value: %d (out of 1023).\n", dac_value);

  // // Call the write function to send the data
  // if (!write_register(upper_byte, lower_byte))
  // {
  //   printf("[DAC] ERROR: Failed to write voltage data to DAC.\n");
  // }
  // else
  // {
  //   printf("[DAC] Voltage set successfully.\n");
  // }
}
