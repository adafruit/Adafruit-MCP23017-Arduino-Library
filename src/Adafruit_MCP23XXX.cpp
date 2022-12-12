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
  @param _hw_addr Hardware address (pins A2, A1, A0)
  @return true if initialization successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_MCP23XXX::begin_SPI(uint8_t cs_pin, SPIClass *theSPI,
                                  uint8_t _hw_addr) {
  this->hw_addr = _hw_addr;
  spi_dev = new Adafruit_SPIDevice(cs_pin, 1000000, SPI_BITORDER_MSBFIRST,
                                   SPI_MODE0, theSPI);
  return spi_dev->begin();
}

/**************************************************************************/
/*!
  @brief Initialize MCP using software SPI.
  @param cs_pin Pin to use for SPI chip select
  @param sck_pin Pin to use for SPI clock
  @param miso_pin Pin to use for SPI MISO
  @param mosi_pin Pin to use for SPI MOSI
  @param _hw_addr Hardware address (pins A2, A1, A0)
  @return true if initialization successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_MCP23XXX::begin_SPI(int8_t cs_pin, int8_t sck_pin,
                                  int8_t miso_pin, int8_t mosi_pin,
                                  uint8_t _hw_addr) {
  this->hw_addr = _hw_addr;
  spi_dev = new Adafruit_SPIDevice(cs_pin, sck_pin, miso_pin, mosi_pin);
  return spi_dev->begin();
}

/**************************************************************************/
/*!
  @brief Configures the specified pin to behave either as an input or an
  output.
  @param pin the Arduino pin number to set the mode of
  @param mode INPUT, OUTPUT, or INPUT_PULLUP
*/
/**************************************************************************/
void Adafruit_MCP23XXX::pinMode(uint8_t pin, uint8_t mode) {
  Adafruit_BusIO_Register IODIR(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                getRegister(MCP23XXX_IODIR, MCP_PORT(pin)));
  Adafruit_BusIO_Register GPPU(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                               getRegister(MCP23XXX_GPPU, MCP_PORT(pin)));
  Adafruit_BusIO_RegisterBits dir_bit(&IODIR, 1, pin % 8);
  Adafruit_BusIO_RegisterBits pullup_bit(&GPPU, 1, pin % 8);

  dir_bit.write((mode == OUTPUT) ? 0 : 1);
  pullup_bit.write((mode == INPUT_PULLUP) ? 1 : 0);
}

/**************************************************************************/
/*!
  @brief Reads the value from a specified digital pin, either HIGH or LOW.
  @param pin the Arduino pin number you want to read
  @returns HIGH or LOW
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::digitalRead(uint8_t pin) {
  Adafruit_BusIO_Register GPIO(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                               getRegister(MCP23XXX_GPIO, MCP_PORT(pin)));
  Adafruit_BusIO_RegisterBits pin_bit(&GPIO, 1, pin % 8);

  return ((pin_bit.read() == 0) ? LOW : HIGH);
}

/**************************************************************************/
/*!
  @brief Write a HIGH or a LOW value to a digital pin.
  @param pin the Arduino pin number
  @param value HIGH or LOW
*/
/**************************************************************************/
void Adafruit_MCP23XXX::digitalWrite(uint8_t pin, uint8_t value) {
  Adafruit_BusIO_Register GPIO(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                               getRegister(MCP23XXX_GPIO, MCP_PORT(pin)));
  Adafruit_BusIO_RegisterBits pin_bit(&GPIO, 1, pin % 8);

  pin_bit.write((value == LOW) ? 0 : 1);
}

/**************************************************************************/
/*!
  @brief Bulk read all pins on a port.
  @param port 0 for Port A, 1 for Port B (MCP23X17 only).
  @returns current pin states of port as a uint8_t.
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::readGPIO(uint8_t port) {
  Adafruit_BusIO_Register GPIO(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                               getRegister(MCP23XXX_GPIO, port));
  return GPIO.read() & 0xFF;
}

/**************************************************************************/
/*!
  @brief Bulk write all pins on a port.
  @param value pin states to write as a uint8_t.
  @param port 0 for Port A, 1 for Port B (MCP23X17 only).
*/
/**************************************************************************/
void Adafruit_MCP23XXX::writeGPIO(uint8_t value, uint8_t port) {
  Adafruit_BusIO_Register GPIO(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                               getRegister(MCP23XXX_GPIO, port));
  GPIO.write(value);
}

/**************************************************************************/
/*!
  @brief Configure the interrupt system.
  @param mirroring true to OR both INTA and INTB pins.
  @param openDrain true for open drain output, false for active drive output.
  @param polarity HIGH or LOW
*/
/**************************************************************************/
void Adafruit_MCP23XXX::setupInterrupts(bool mirroring, bool openDrain,
                                        uint8_t polarity) {
  Adafruit_BusIO_Register IOCON(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                getRegister(MCP23XXX_IOCON));
  Adafruit_BusIO_RegisterBits mirror_bit(&IOCON, 1, 6);
  Adafruit_BusIO_RegisterBits openDrain_bit(&IOCON, 1, 2);
  Adafruit_BusIO_RegisterBits polarity_bit(&IOCON, 1, 1);

  mirror_bit.write(mirroring ? 1 : 0);
  openDrain_bit.write(openDrain ? 1 : 0);
  polarity_bit.write((polarity == HIGH) ? 1 : 0);
}

/**************************************************************************/
/*!
  @brief Enable interrupt and set mode for given pin.
  @param pin Pin to enable.
  @param mode CHANGE, LOW, HIGH
*/
/**************************************************************************/
void Adafruit_MCP23XXX::setupInterruptPin(uint8_t pin, uint8_t mode) {
  Adafruit_BusIO_Register GPINTEN(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                  getRegister(MCP23XXX_GPINTEN, MCP_PORT(pin)));
  Adafruit_BusIO_Register INTCON(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                 getRegister(MCP23XXX_INTCON, MCP_PORT(pin)));
  Adafruit_BusIO_Register DEFVAL(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                 getRegister(MCP23XXX_DEFVAL, MCP_PORT(pin)));
  Adafruit_BusIO_RegisterBits enable_bit(&GPINTEN, 1, pin % 8);
  Adafruit_BusIO_RegisterBits config_bit(&INTCON, 1, pin % 8);
  Adafruit_BusIO_RegisterBits defval_bit(&DEFVAL, 1, pin % 8);

  enable_bit.write(1);                        // enable it
  config_bit.write((mode == CHANGE) ? 0 : 1); // set mode
  defval_bit.write((mode == LOW) ? 1 : 0);    // set defval
}

/**************************************************************************/
/*!
  @brief Disable interrupt for given pin.
  @param pin Pin to disable.
*/
/**************************************************************************/
void Adafruit_MCP23XXX::disableInterruptPin(uint8_t pin) {
  Adafruit_BusIO_Register GPINTEN(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                  getRegister(MCP23XXX_GPINTEN, MCP_PORT(pin)));
  Adafruit_BusIO_RegisterBits enable_bit(&GPINTEN, 1, pin % 8);

  enable_bit.write(0);
}

/**************************************************************************/
/*!
  @brief Clear interrupts. NOTE:If using DEFVAL, INT clears only if interrupt
  condition does not exist. See Fig 1-7 in datasheet.
*/
/**************************************************************************/
void Adafruit_MCP23XXX::clearInterrupts() {
  // reading INTCAP register(s) clears interrupts
  // just call this and ignore return
  getCapturedInterrupt();
}

/**************************************************************************/
/*!
  @brief Gets the pin that caused the latest interrupt, from INTF, without
  clearing any interrupt flags.
  @returns Pin that caused lastest interrupt.
*/
/**************************************************************************/
uint8_t Adafruit_MCP23XXX::getLastInterruptPin() {
  uint8_t intf;

  // Port A
  Adafruit_BusIO_Register INTFA(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                getRegister(MCP23XXX_INTF, 0));
  INTFA.read(&intf);
  for (uint8_t pin = 0; pin < 8; pin++) {
    if (intf & (1 << pin)) {
      return pin;
    }
  }

  // Port B ?
  if (pinCount > 8) {
    Adafruit_BusIO_Register INTFB(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                  getRegister(MCP23XXX_INTF, 1));
    INTFB.read(&intf);
    for (uint8_t pin = 0; pin < 8; pin++) {
      if (intf & (1 << pin)) {
        return pin + 8;
      }
    }
  }

  return MCP23XXX_INT_ERR;
}

/**************************************************************************/
/*!
  @brief Get pin states captured at time of interrupt.
  @returns Mutli-bit value representing pin states.
*/
/**************************************************************************/
uint16_t Adafruit_MCP23XXX::getCapturedInterrupt() {
  uint16_t intcap;
  uint8_t intf;

  // Port A
  Adafruit_BusIO_Register INTCAPA(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                  getRegister(MCP23XXX_INTCAP, 0));
  INTCAPA.read(&intf);
  intcap = intf;

  // Port B ?
  if (pinCount > 8) {
    Adafruit_BusIO_Register INTCAPB(i2c_dev, spi_dev, MCP23XXX_SPIREG,
                                    getRegister(MCP23XXX_INTCAP, 1));
    INTCAPB.read(&intf);
    intcap |= (uint16_t)intf << 8;
  }

  return intcap;
}

/**************************************************************************/
/*!
  @brief helper to get register address
  @param baseAddress base register address
  @param port 0 for A, 1 for B (MCP23X17 only)
  @returns calculated register address
*/
/**************************************************************************/
uint16_t Adafruit_MCP23XXX::getRegister(uint8_t baseAddress, uint8_t port) {
  // MCP23x08
  uint16_t reg = baseAddress;
  // MCP23x17 BANK=0
  if (pinCount > 8) {
    reg <<= 1;
    reg |= port;
  }
  // for SPI, add opcode as high byte
  return (spi_dev) ? (0x4000 | (hw_addr << 9) | reg) : reg;
}
