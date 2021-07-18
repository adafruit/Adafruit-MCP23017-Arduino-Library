/*!
 * @file Adafruit_MCP23X17.cpp
 */

#include "Adafruit_MCP23X17.h"

/**************************************************************************/
/*!
  @brief default ctor.
*/
/**************************************************************************/
Adafruit_MCP23X17::Adafruit_MCP23X17() { pinCount = 16; }

/**************************************************************************/
/*!
  @brief Bulk read all pins on Port A.
  @returns current pin states of port as a uint8_t.
*/
/**************************************************************************/
uint8_t Adafruit_MCP23X17::readGPIOA() { return readGPIO(0); }

/**************************************************************************/
/*!
  @brief Bulk write all pins on Port A.
  @param value pin states to write as uint8_t.
*/
/**************************************************************************/
void Adafruit_MCP23X17::writeGPIOA(uint8_t value) { writeGPIO(value, 0); }

/**************************************************************************/
/*!
  @brief Bulk read all pins on Port B.
  @returns current pin states of port as a uint8_t.
*/
/**************************************************************************/
uint8_t Adafruit_MCP23X17::readGPIOB() { return readGPIO(1); }

/**************************************************************************/
/*!
  @brief Bulk write all pins on Port B.
  @param value pin states to write as uint8_t.
*/
/**************************************************************************/
void Adafruit_MCP23X17::writeGPIOB(uint8_t value) { writeGPIO(value, 1); }

/**************************************************************************/
/*!
  @brief Bulk read all pins on Port A and B.
  @returns current pin states of ports as a uint16_t.
*/
/**************************************************************************/
uint16_t Adafruit_MCP23X17::readGPIOAB() {
  Adafruit_BusIO_Register GPIO(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                               getRegister(MCP23XXX_GPIO, 0), 2);
  return GPIO.read();
}

/**************************************************************************/
/*!
  @brief Bulk write all pins on Port A and Port B.
  @param value pin states to write as uint16_t.
*/
/**************************************************************************/
void Adafruit_MCP23X17::writeGPIOAB(uint16_t value) {
  Adafruit_BusIO_Register GPIO(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                               getRegister(MCP23XXX_GPIO, 0), 2);
  GPIO.write(value, 2);
}
