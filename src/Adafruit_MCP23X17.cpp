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

/**************************************************************************/
/*!
  @brief Enable usage of HW address pins (A0, A1, A2) on MCP23S17

  Send this message as first message after chip init, as it will
  set bits in IOCON register to default (except HAEN)
  By default pins are not used and disabled (see README for details)
  This message is sent to all devices on bus (no hw_addr is added to msg
  as it's not enabled yet)
  Due to HW bug in the chip message must be sent twice (to addr 0b000 and
  0b1xx)
*/
/**************************************************************************/
void Adafruit_MCP23X17::enableAddrPins() {
  if (!spi_dev) // I2C dev always use addr, only makes sense for SPI dev
    return;

  Adafruit_BusIO_Register GPIOAddr(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                   getRegister(MCP23XXX_IOCON, 0), 2);

  // Send message to address 0b000 regardless of chip addr,
  // Because addressing is not yet enabled
  uint8_t tmp = this->hw_addr;
  this->hw_addr = 0; // Temporary set hw addr to 0
  Adafruit_BusIO_Register GPIONoAddr(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                     getRegister(MCP23XXX_IOCON, 0), 2);
  this->hw_addr = tmp;

  GPIONoAddr.write((1 << 3), 1); // Bit3: HAEN, devices with A2 = 0
  GPIOAddr.write((1 << 3), 1);   // Devices with A2 = 1 (if any)
}