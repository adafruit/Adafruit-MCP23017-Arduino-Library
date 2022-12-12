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

| MCP23x08 Pin # | MCP23x17 Pin # | Pin Name | Pin ID |
| :------------: | :------------: | :------: | :----: |
|       10       |       21       |   GPA0   |   0    |
|       11       |       22       |   GPA1   |   1    |
|       12       |       23       |   GPA2   |   2    |
|       13       |       24       |   GPA3   |   3    |
|       14       |       25       |   GPA4   |   4    |
|       15       |       26       |   GPA5   |   5    |
|       16       |       27       |   GPA6   |   6    |
|       17       |       28       |   GPA7   |   7    |
|       --       |       1        |   GPB0   |   8    |
|       --       |       2        |   GPB1   |   9    |
|       --       |       3        |   GPB2   |   10   |
|       --       |       4        |   GPB3   |   11   |
|       --       |       5        |   GPB4   |   12   |
|       --       |       6        |   GPB5   |   13   |
|       --       |       7        |   GPB6   |   14   |
|       --       |       8        |   GPB7   |   15   |

# Use of HW address pins for SPI device

Library supports MCP23Sxx HW pin addressing (A2, A1, A0 for S17 and A1, A0 for S08)
To use it provide HW address to begin_SPI(CS, SPI, HW_ADDR) function, and as a result each SPI message will contain correct chip address.

Example:
mcp.begin_SPI(10, &SPI, 0b101);

HW Address recognition must be enabled by enableAddrPins() function. **NOTE** Calling enableAddrPins() will enable IOCON.HAEN bit for all active (CS low) devices on SPI bus.
**NOTE**
There is hardware bug in the MCP23S17 chip, see "MCP23S17 Rev. A Silicon Errata".
As a result, if using device with A2 = high, and not using addressing, hw address must be set to 0b1XX
In such case, even if not using addressing, initalize your MCP23S17 chip with 0b1XX address, eg: mcp.begin_SPI(10, &SPI, 0b100);.

# Warning

Some people have reported an undocumented bug that can potentially corrupt the I2C bus.
It occurs if an MCP230XX input pin state changes during I2C readout. **This should be very rare.** For more information, see this [forum post](https://www.microchip.com/forums/m646539.aspx) and this [knowledge base article](https://microchipsupport.force.com/s/article/On-MCP23008-MCP23017-SDA-line-change-when-GPIO7-input-change>).
