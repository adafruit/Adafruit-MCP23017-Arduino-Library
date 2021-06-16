/*!
 * @file Adafruit_MCP23XXX.cpp
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

#include "Adafruit_MCP23XXX.h"

/**************************************************************************/
/*!
  @brief Initialize MCP using I2C.
  @param i2c_addr I2C address
  @param wire Pointer to Wire instance
  @return true if initialization successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_MCP23XXX::begin_I2C(uint8_t i2c_addr, TwoWire *wire) {
  i2c_dev = new Adafruit_I2CDevice(i2c_addr, wire);
  return i2c_dev->begin();
}

/**************************************************************************/
/*!
  @brief Initialize MCP using hardware SPI.
  @param cs_pin Pin to use for SPI chip select
  @param theSPI Pointer to SPI instance
  @return true if initialization successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_MCP23XXX::begin_SPI(uint8_t cs_pin, SPIClass *theSPI) {
  spi_dev = new Adafruit_SPIDevice(cs_pin, 1000000, SPI_BITORDER_MSBFIRST, SPI_MODE0, theSPI);
  return spi_dev->begin();
}

/**************************************************************************/
/*!
  @brief Initialize MCP using software SPI.
  @param cs_pin Pin to use for SPI chip select
  @param sck_pin Pin to use for SPI clock
  @param miso_pin Pin to use for SPI MISO
  @param mosi_pin Pin to use for SPI MOSI
  @return true if initialization successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_MCP23XXX::begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
                 int8_t mosi_pin) {
  spi_dev = new Adafruit_SPIDevice(cs_pin, sck_pin, miso_pin, mosi_pin);
  return spi_dev->begin();
}

/**************************************************************************/
/*!
  @brief Configures the specified pin to behave either as an input or an output.
  @param pin the Arduino pin number to set the mode of
  @param mode INPUT, OUTPUT, or INPUT_PULLUP
*/
/**************************************************************************/
void Adafruit_MCP23XXX::pinMode(uint8_t pin, uint8_t mode) {
  uint8_t iodir_reg = getRegister(MCP23XXX_IODIR, PORT(pin));
  uint8_t gppu_reg = getRegister(MCP23XXX_GPPU, PORT(pin));

  uint8_t iodir = readRegister(iodir_reg);

  if (mode == OUTPUT) {
    // clear for output
    iodir &= ~MASK(pin);
  } else {
    // set for input
    iodir |= MASK(pin);
    // also configure internal pull-up
    uint8_t gppu = readRegister(gppu_reg);
    if (mode == INPUT_PULLUP) {
      // set to enable
      gppu |= MASK(pin);
    } else {
      // clear to disable
      gppu &= ~MASK(pin);
    }
    writeRegister(gppu_reg, gppu);
  }
  writeRegister(iodir_reg, iodir);
}

/**************************************************************************/
/*!
  @brief Reads the value from a specified digital pin, either HIGH or LOW.
  @param pin the Arduino pin number you want to read
  @returns HIGH or LOW
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::digitalRead(uint8_t pin) {
  if (pin >= pinCount) return 0;
  return ((readGPIO(PORT(pin)) & MASK(pin)) == 0) ? LOW : HIGH;
}

/**************************************************************************/
/*!
  @brief Write a HIGH or a LOW value to a digital pin.
  @param pin the Arduino pin number
  @param value HIGH or LOW
*/
/**************************************************************************/
void Adafruit_MCP23XXX::digitalWrite(uint8_t pin, uint8_t value) {
  uint8_t gpio = readGPIO(PORT(pin));
  if (value == HIGH) {
    gpio |= MASK(pin);
  } else {
    gpio &= ~MASK(pin);
  }
  writeGPIO(gpio, PORT(pin));
}

/**************************************************************************/
/*!
  @brief Bulk read all pins on a port.
  @param port 0 for Port A, 1 for Port B (MCP23X17 only).
  @returns current pin states of port as a uint8_t.
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::readGPIO(uint8_t port) {
  return readRegister(getRegister(MCP23XXX_GPIO, port));
}

/**************************************************************************/
/*!
  @brief Bulk write all pins on a port.
  @param value pin states to write as a uint8_t.
  @param port 0 for Port A, 1 for Port B (MCP23X17 only).
*/
/**************************************************************************/
void Adafruit_MCP23XXX::writeGPIO(uint8_t value, uint8_t port) {
  writeRegister(getRegister(MCP23XXX_GPIO, port), value);
}

/**************************************************************************/
/*!
  @brief Configure the interrupt system.
  @param mirroring true to OR both INTA and INTB pins.
  @param open true for open drain output, false for active drive output.
  @param polarity HIGH or LOW
*/
/**************************************************************************/
void Adafruit_MCP23XXX::setupInterrupts(bool mirroring, bool openDrain, uint8_t polarity) {
  uint8_t iocon = readRegister(getRegister(MCP23XXX_IOCON));
  if (mirroring) iocon |= 1 << 6; else iocon &= ~(1 << 6);
  if (openDrain) iocon |= 1 << 2; else iocon &= ~(1 << 2);
  if (polarity == HIGH) iocon |= 1 << 1; else iocon &= ~(1 << 1);
  writeRegister(getRegister(MCP23XXX_IOCON), iocon);
}

/**************************************************************************/
/*!
  @brief Enable interrupt and set mode for given pin.
  @param pin Pin to enable.
  @param mode CHANGE, LOW, HIGH
*/
/**************************************************************************/
void Adafruit_MCP23XXX::setupInterruptPin(uint8_t pin, uint8_t mode) {
  // enable it
  uint8_t reg = getRegister(MCP23XXX_GPINTEN, PORT(pin));
  uint8_t gpinten = readRegister(reg);
  gpinten |= MASK(pin);
  writeRegister(reg, gpinten);
  // set mode
  reg = getRegister(MCP23XXX_INTCON, PORT(pin));
  uint8_t intcon = readRegister(reg);
  if (mode == CHANGE) {
    // clear to compare to previous self (CHANGE)
    intcon &= ~MASK(pin);
    writeRegister(reg, intcon);
  } else {
    // set to compare to DEFVAL (LOW/HIGH)
    intcon |= MASK(pin);
    writeRegister(reg, intcon);
    // set DEFVAL to 1=LOW or 0=HIGH
    reg = getRegister(MCP23XXX_DEFVAL, PORT(pin));
    uint8_t defval = readRegister(reg);
    if (mode == LOW) defval |= MASK(pin); else defval &= ~MASK(pin);
    writeRegister(reg, defval);
  }
}

/**************************************************************************/
/*!
  @brief Disable interrupt for given pin.
  @param pin Pin to disable.
*/
/**************************************************************************/
void Adafruit_MCP23XXX::disableInterruptPin(uint8_t pin) {
  uint8_t reg = getRegister(MCP23XXX_GPINTEN, PORT(pin));
  uint8_t gpinten = readRegister(reg);
  gpinten &= ~MASK(pin);
  writeRegister(reg, gpinten);
}

/**************************************************************************/
/*!
  @brief Gets the last interrupt pin.
  @returns Pin that caused last interrupt.
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::getLastInterruptPin() {
  uint8_t intf = readRegister(getRegister(MCP23XXX_INTF));
  uint8_t intpin = 255;
  // Port A
  for (uint8_t pin = 0; pin < 8; pin++) {
    if (intf & (1 << pin)) {
      intpin = pin;
      break;
    }
  }
  // Port B
  if ((pinCount > 8) && (intpin != 255)) {
    intf = readRegister(getRegister(MCP23XXX_INTF, 1));
    for (uint8_t pin = 0; pin < 8; pin++) {
      if (intf & (1 << pin)) {
        intpin = pin + 8;
        break;
      }
    }
  }
  // read INTCAP to clear
  readRegister(getRegister(MCP23XXX_INTCAP));
  return intpin;
}

/**************************************************************************/
/*!
  @brief read register
  @param addr register address
  @returns register value
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::readRegister(uint8_t addr) {
  if (i2c_dev) {
    buffer[0] = addr;
    i2c_dev->write_then_read(buffer, 1, buffer, 1, false);
  } else if (spi_dev) {
    buffer[0] = MCP23XXX_SPI_READ;
    buffer[1] = addr;
    spi_dev->write_then_read(buffer, 2, buffer, 1);
  }
  return buffer[0];
}

/**************************************************************************/
/*!
  @brief write register
  @param addr register address
  @param value value to write
*/
/**************************************************************************/
void Adafruit_MCP23XXX::writeRegister(uint8_t addr, uint8_t value) {
  if (i2c_dev) {
    buffer[0] = addr;
    buffer[1] = value;
    i2c_dev->write(buffer, 2);
  } else if (spi_dev) {
    buffer[0] = MCP23XXX_SPI_WRITE;
    buffer[1] = addr;
    buffer[2] = value;
    spi_dev->write(buffer, 3);
  }
}

/**************************************************************************/
/*!
  @brief read two consecutive registers
  @param addr first register address
  @returns register values, first register in lower byte
*/
/**************************************************************************/
uint16_t Adafruit_MCP23XXX::readRegister16(uint8_t addr) {
  if (i2c_dev) {
    buffer[0] = addr;
    i2c_dev->write_then_read(buffer, 1, buffer, 2, false);
  } else if (spi_dev) {
    buffer[0] = MCP23XXX_SPI_READ;
    buffer[1] = addr;
    spi_dev->write_then_read(buffer, 2, buffer, 2);
  }
  return buffer[0] | (buffer[1] << 1);
}

/**************************************************************************/
/*!
  @brief write two consecutive registers
  @param addr first register address
  @param value register values, first register in lower byte
*/
/**************************************************************************/
void Adafruit_MCP23XXX::writeRegister16(uint8_t addr, uint16_t value) {
  if (i2c_dev) {
    buffer[0] = addr;
    buffer[1] = value & 0xFF;
    buffer[2] = (value >> 8) & 0xFF;
    i2c_dev->write(buffer, 3);
  } else if (spi_dev) {
    buffer[0] = MCP23XXX_SPI_WRITE;
    buffer[1] = addr;
    buffer[2] = value & 0xFF;
    buffer[3] = (value >> 8) & 0xFF;
    spi_dev->write(buffer, 4);
  }
}

/**************************************************************************/
/*!
  @brief helper to get register address
  @param baseAddress base register address
  @param port 0 for A, 1 for B (MCP23X17 only)
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::getRegister(uint8_t baseAddress, uint8_t port) {
  // MCP23x08
  uint8_t reg = baseAddress;
  // MCP23x17 BANK=0
  if (pinCount > 8) {
    reg *= 2;
    // Port B
    if (port) reg++;
  }
  return reg;
}