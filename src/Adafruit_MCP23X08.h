/*!
 * @file Adafruit_MCP23X08.h
 */

#ifndef __ADAFRUIT_MCP23X08_H__
#define __ADAFRUIT_MCP23X08_H__

#include "Adafruit_MCP23XXX.h"

/**************************************************************************/
/*!
    @brief  Class for MCP23008 I2C and MCP23S08 SPI variants.
*/
/**************************************************************************/
class Adafruit_MCP23X08 : public Adafruit_MCP23XXX {
public:
  Adafruit_MCP23X08();

  void enableAddrPins();
};

#endif