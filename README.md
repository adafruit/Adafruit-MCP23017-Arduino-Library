# Adafruit MCP23017 Arduino Library [![Build Status](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](http://adafruit.github.io/Adafruit-MCP23017-Arduino-Library/html/index.html)

This is a library for the MCP23008/17 I2C and MCP23S08/17 SPI Port Expanders.

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Carter Nelson for Adafruit Industries.
BSD license, check license.txt for more information
All text above must be included in any redistribution

To install, use the Arduino IDE Library Manager.

# Pin Addressing

When using single pin operations such as _pinMode(pinId, dir)_ or _digitalRead(pinId)_  or _digitalWrite(pinId, val)_ then the pins are addressed using the ID's below. For example, for set the mode of _GPB0_ then use _pinMode(8, ...)_. **NOTE** The MCP23008 and MCP23S08 only have _GPAx_ pins.

MCP23x08 Pin # | MCP23x17 Pin # | Pin Name | Pin ID
:-------------:|:--------------:|:--------:|:-------:
10 | 21 | GPA0 | 0
11 | 22 | GPA1 | 1
12 | 23 | GPA2 | 2
13 | 24 | GPA3 | 3
14 | 25 | GPA4 | 4
15 | 26 | GPA5 | 5
16 | 27 | GPA6 | 6
17 | 28 | GPA7 | 7
-- |  1 | GPB0 |  8
-- |  2 | GPB1 |  9
-- |  3 | GPB2 | 10
-- |  4 | GPB3 | 11
-- |  5 | GPB4 | 12
-- |  6 | GPB5 | 13
-- |  7 | GPB6 | 14
-- |  8 | GPB7 | 15
