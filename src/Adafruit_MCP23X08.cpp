/*!
 * @file Adafruit_MCP23X08.cpp
 */

#include "Adafruit_MCP23X08.h"

/**************************************************************************/
/*!
  @brief default ctor.
*/
/**************************************************************************/
Adafruit_MCP23X08::Adafruit_MCP23X08() { pinCount = 8; }

/**************************************************************************/
/*!
  @brief Enable usage of HW address pins (A0, A1) on MCP23S08

  Send this message as first message after chip init.

  By default HW address pins are disabled.
  (Register IOCON, bit HAEN = 0 on chip reset)
*/
/**************************************************************************/
void Adafruit_MCP23X08::enableAddrPins() {
  if (!spi_dev) // I2C dev always use addr, only makes sense for SPI dev
    return;

  Adafruit_BusIO_Register GPIOAddr(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                   getRegister(MCP23XXX_IOCON, 0), 2);

  GPIOAddr.write((1 << 3), 1);
}