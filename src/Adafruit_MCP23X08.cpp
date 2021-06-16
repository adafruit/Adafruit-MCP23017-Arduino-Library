/*!
 * @file Adafruit_MCP23X08.cpp
 *
 * @mainpage Adafruit MCP23X08/17 Library
 *
 * @section intro_sec Introduction
 *
 * This is a library for the MCP23008/17 I2C and MCP23S08/17 SPI port
 * expanders.
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Carter Nelson for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution
 */

#include "Adafruit_MCP23X08.h"

/**************************************************************************/
/*!
  @brief default ctor.
*/
/**************************************************************************/
Adafruit_MCP23X08::Adafruit_MCP23X08() { pinCount = 8; }
