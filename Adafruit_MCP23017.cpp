/*************************************************** 
  This is a library for the MCP23017 i2c port expander

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
    ------------------------------------------------------------------
  FORKED by Red Byer (www.redstoyland.com) on 7/29/2013
     Adding features and controls,
     -  Interrupt enabling
     -  Single port control
     https://github.com/mizraith/mizraith-MCP23017.git
   
 ****************************************************/

#include <Wire.h>
#include <avr/pgmspace.h>
#include "Adafruit_MCP23017.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// minihelper
static inline void wiresend(uint8_t x) {
#if ARDUINO >= 100
  Wire.write((uint8_t)x);
#else
  Wire.send(x);
#endif
}

static inline uint8_t wirerecv(void) {
#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Takes address of A2A1A0 and ORs that with the MCP23017 address
 * to begin i2C transmission.
 *
 * Starts by setting all ports to input mode (less likely to cause harm).
 */
void Adafruit_MCP23017::begin(uint8_t addr) {
  uint8_t temp = 0;
  
  if (addr > 7) {
    addr = 7;
  }
  i2caddr = addr;

  Wire.begin();

  
  // set defaults -- THIS IS REDUNDANT
  // the IODIR registers power up defaulted as inputs anyway
  //       1 = input  0 = output
  setRegister(MCP23017_IODIRA, 0xFF); // all inputs on port A
  setRegister(MCP23017_IODIRB, 0xFF); // all inputs on port B

  //set up our IOCONA and IOCONB registers for basic operation
  temp = temp &  ~(1 << MCP23017_BIT_BANK);     //Make sure BANK = 0
  //temp = temp | (1 << MCP23017_BIT_MIRROR);  //INT mirroring
  temp = temp &  ~(1 << MCP23017_BIT_SEQOP );   // IMPORTANT, set to 0 for sequential op
  //temp = temp | (1 << MCP23017_BIT_DISSLW);  //INT mirroring
  temp = temp |   (1 << MCP23017_BIT_HAEN);    // IMPORTANT to enable address pins
  temp = temp &  ~(1 << MCP23017_BIT_ODR);     //  0=active driver output, disable open drain
  temp = temp |   (1 << MCP23017_BIT_INTPOL);  // Set int pin to Active High
  
  // temp should be:       0000 1010
  setRegister(MCP23017_IOCONA, temp);  //theoretically IOCONA and IOCONB are shared


}


/**
 * Shortcut begin() method that assumes address 000 (A2A1A0)
 */
void Adafruit_MCP23017::begin(void) {
  begin(0);
}

/********************************************************
* PORT MODE AND PULL UP FUNCTIONS
*********************************************************/

/**
 *  Sets the a given bit (0:15) to either INPUT or OUTPUT
 *  @param p is the bit (0:15) where values <8 below to port A
 *  @param d is the direction for that bit
 *       1 = input  0 = output
 */
void Adafruit_MCP23017::setPinMode(uint8_t p, uint8_t d) {
  uint8_t iodir;
  uint8_t iodiraddr;

  // only 16 bits!
  if (p > 15)
    return;

  if (p < 8)
    iodiraddr = MCP23017_IODIRA;
  else {
    iodiraddr = MCP23017_IODIRB;
    p -= 8;
  }

  // read the current IODIR
  iodir = getRegister(iodiraddr);
  
  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p; 
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  setRegister(iodiraddr, iodir);
}

/**
 * Set the input/output mode for GPIOA in one operation
 *       1 = input  0 = output
 */
void Adafruit_MCP23017::setGPIOAMode(uint8_t d) {
  setRegister(MCP23017_IODIRA, d);
}

/**
 * Set the input/output mode for GPIOA in one operation
 *       1 = input  0 = output
 */
void Adafruit_MCP23017::setGPIOBMode(uint8_t d) {
  setRegister(MCP23017_IODIRB, d);
}

/**
 * Set the input/output mode for GPIO A and B in one operation
 * Port A is the lower byte.  Assumes sequential mode of operation
 *       1 = input  0 = output
 */
void Adafruit_MCP23017::setGPIOABMode(uint16_t d) {
  set2Registers(MCP23017_IODIRA, d);
}



/**
 * Set pullup for a single bit [0:15] of the 16 GPIOs.
 * Port A pins are < 8.
 */
void Adafruit_MCP23017::setPinPullUp(uint8_t p, uint8_t d) {
  uint8_t gppu;
  uint8_t gppuaddr;

  // only 16 bits!
  if (p > 15)
    return;

  if (p < 8)
    gppuaddr = MCP23017_GPPUA;
  else {
    gppuaddr = MCP23017_GPPUB;
    p -= 8;
  }

  // read the current pullup resistor set
  gppu = getRegister(gppuaddr);

  // set the pin and direction
  if (d == HIGH) {
    gppu |= 1 << p; 
  } else {
    gppu &= ~(1 << p);
  }

  // write the new GPIO
  setRegister(gppuaddr, gppu);
}

/**
 *Set pullups on port A to 'd' in a single function.
 */
void Adafruit_MCP23017::setGPIOAPullUp(uint8_t d) {
	setRegister(MCP23017_GPPUA, d);
}
/**
 *Set pullups on port B to 'd' in a single function.
 */
void Adafruit_MCP23017::setGPIOBPullUp(uint8_t d) {
	setRegister(MCP23017_GPPUB, d);
}
/**
 *Set pullups on ports A and B to 'd' in a single function.
 */
void Adafruit_MCP23017::setGPIOABPullUp(uint16_t d) {
	set2Registers(MCP23017_GPPUA, d);
}



/********************************************************
* PORT READ FUNCTIONS
*********************************************************/
/**
 * Read a single pin's value [0:15]
 * Port A pins are < 8.
 */
uint8_t Adafruit_MCP23017::readPin(uint8_t p) {
  uint8_t gpioaddr, val;

  // only 16 bits!
  if (p > 15)
    return 0;

  if (p < 8)
    gpioaddr = MCP23017_GPIOA;
  else {
    gpioaddr = MCP23017_GPIOB;
    p -= 8;
  }

  // read the current GPIO
  val = getRegister(gpioaddr);	
  
  return (val >> p) & 0x1;
}


/**
 * Read only port A
 */
uint8_t Adafruit_MCP23017::readGPIOA() {
  return getRegister(MCP23017_GPIOA);
}

/**
 * Read only port B
 */
uint8_t Adafruit_MCP23017::readGPIOB() {
  return getRegister(MCP23017_GPIOB);
}

/**
 * Reads both GPIOA and GPIOB
 * Return uint16 wtih A in the lower byte.
 * Note, this assuems SEQUENTIAL (SEQOP) bit is set.
 */
uint16_t Adafruit_MCP23017::readGPIOAB() {
	return get2Registers(MCP23017_GPIOA);
}
//-----was as follows-------------
//  uint16_t ba = 0;
//  uint8_t a;
//
//  // read the current GPIO output latches
//  Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
//  wiresend(MCP23017_GPIOA);	
//  Wire.endTransmission();
//  
//  Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 2);
//  a = wirerecv();
//  ba = wirerecv();
//  ba <<= 8;
//  ba |= a;
//
//  return ba;


/********************************************************
*  PORT WRITE FUNCTIONS
*********************************************************/

/**
 * Write a single bit [0:15] of the 16 GPIOs as either HIGH or LOW.
 * Port A pins are values < 8.
 */
void Adafruit_MCP23017::writePin(uint8_t p, uint8_t d) {
  uint8_t gpio;
  uint8_t gpioaddr, olataddr;

  // only 16 bits!
  if (p > 15)
    return;

  if (p < 8) {
    olataddr = MCP23017_OLATA;
    gpioaddr = MCP23017_GPIOA;
  } else {
    olataddr = MCP23017_OLATB;
    gpioaddr = MCP23017_GPIOB;
    p -= 8;
  }

  // read the current GPIO output latches
  gpio = getRegister(olataddr);

  // set the pin and direction
  if (d == HIGH) {
    gpio |= 1 << p; 
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  setRegister(gpioaddr, gpio);
}

/**
 * Write val to port A only
 */ 
void Adafruit_MCP23017::writeGPIOA(uint8_t val) {
	setRegister(MCP23017_GPIOA, val);
}
/**
 * Write val to port B only
 */
void Adafruit_MCP23017::writeGPIOB(uint8_t val) {
	setRegister(MCP23017_OLATB, val);
}
/**
 * Takes 16 bit value and writes to both port A and B
 *   PORT A is represented in low byte
 * Assumes sequential mode of operation
 */
void Adafruit_MCP23017::writeGPIOAB(uint16_t ba) {
  set2Registers(MCP23017_GPIOA, ba);	
}



/********************************************************
* INTERRUPT FUNCTIONALITY
*********************************************************/


//-----------INTERRUPT: HIGHER LEVEL SETTINGS-----------------------------
/**
 * Disable all interrupts on both PortA and PortB.
 * If you want to re-enable, use the setInterrupt function.
 */
void Adafruit_MCP23017::disableInterrupts() {
    set2Registers( MCP23017_GPINTENA, 0x0000 );
}

/**
 * Enable interrupt mirrororing, thereby OR-ing
 * INTA and INTB so that an interrupt on one port
 * shows outputs on both INTx pins.
 * enable=1=INTA || INTB     enable=0=no mirroring
 */
void Adafruit_MCP23017::setInterruptMirror(uint8_t enable) {
  //1= OR INTA/INTB  0= no mirror   
  setBit( MCP23017_IOCONA, MCP23017_BIT_MIRROR, enable);
}

/**
 * Set the interrupt polarity of the INTx pins.
 * 1=activehigh  0=activelow  
 */
void Adafruit_MCP23017::setInterruptPolarity(uint8_t activehigh)  {
//1=activehigh 0=activelow
    setBit( MCP23017_IOCONA, MCP23017_BIT_MIRROR, activehigh);
}

//-----------INTERRUPT: ENABLING/DISABLING INTERRUPTS-----------------------------
/**
 *   Set 'pin' [0:15] to ;enable' state.  1=enable interrupt 0=disable
 *  Port A is 0:7   Port B is 8:15
 **/
void Adafruit_MCP23017::setInterruptPin(uint8_t pin, uint8_t enable ) {
  uint8_t reg;
  uint8_t gpinten;

  // only 16 bits!
  if (pin > 15)
    return;

  if (pin < 8) {
    gpinten = MCP23017_GPINTENA;
  } else {
    gpinten = MCP23017_GPINTENB;
    pin -= 8;
  }

  // read the current GPINTEN
  reg = getRegister( gpinten );

  // set the pin and direction
  if (enable == HIGH) {
    reg |= 1 << pin; 
  } else {
    reg &= ~(1 << pin);
  }

  // write the new reg
  setRegister(gpinten, reg);

}

/**
 *   Set port x's interrupt enable pins to enablemask
 *   1bit=enable int     0bit=disable
 **/
void Adafruit_MCP23017::setInterruptA(uint8_t enablemask) {
  setRegister(MCP23017_GPINTENA, enablemask);
}
void Adafruit_MCP23017::setInterruptB(uint8_t enablemask) {
  setRegister(MCP23017_GPINTENB, enablemask);
}

/**
 *   Set port A and B's interrupt enable pins to enablemask
 *   Port A is lower byte
 *   1bit=enable int     0bit=disable
 *   ASSUMES sequential write operation
 **/
void Adafruit_MCP23017::setInterruptAB(uint16_t enablemask) {
  set2Registers(MCP23017_GPINTENA, enablemask);    
}


//-----------INTERRUPT: HOW PINS INTERRUPT-----------------------------
/**
 * Set how the a given pin triggers an interrupt.
 * interrupts can be copmared against a (1=)default value or just a (0=)pin change
 */
void Adafruit_MCP23017::setInterruptControlPin(uint8_t pin, uint8_t enable ) {
  //INTCONx  1=against defval  0=against previous
  uint8_t reg;
  uint8_t gpintcon;

  // only 16 bits!
  if (pin > 15)
    return;

  if (pin < 8) {
    gpintcon = MCP23017_INTCONA;
  } else {
    gpintcon = MCP23017_INTCONB;
    pin -= 8;
  }

  // read the current INTCON
  reg = getRegister( gpintcon );

  // set the pin and direction
  if (enable == HIGH) {
    reg |= 1 << pin; 
  } else {
    reg &= ~(1 << pin);
  }

  // write the new reg
  setRegister(gpintcon, reg);

}

/**
 * Set how the pins on Port x interrupt based on bits in intconmask
 * interrupts can be copmared against a (1=)default value or just a (0=)pin change
 */
void Adafruit_MCP23017::setInterruptControlA(uint8_t intconmask ) {
  setRegister(MCP23017_INTCONA, intconmask);
}
void Adafruit_MCP23017::setInterruptControlB(uint8_t intconmask ) {
   setRegister(MCP23017_INTCONB, intconmask);
}
/**
 * Set how the pins on Port A and B interrupt based on bits in intconmask
 * interrupts can be copmared against a (1=)default value or just a (0=)pin change
 */
void Adafruit_MCP23017::setInterruptControlAB( uint16_t intconmask ) {
   set2Registers(MCP23017_INTCONA, intconmask);
}


//-----------INTERRUPT: SETTING DEFAULT PIN VALUES-----------------------------
/**
 * Set the default value to check against for interrupting (if set that way)
 * See setInterruptControl functions to set this behavior.
 */
void Adafruit_MCP23017::setDefaultValuePin(uint8_t pin, uint8_t val ) {
  //INTCONx  1=against defval  0=against previous
  uint8_t reg;
  uint8_t addr;

  // only 16 bits!
  if (pin > 15)
    return;

  if (pin < 8) {
    addr = MCP23017_DEFVALA;
  } else {
    addr = MCP23017_DEFVALB;
    pin -= 8;
  }

  // read the current DEFVALx
  reg = getRegister( addr );

  // set the pin and direction
  if (val == HIGH) {
    reg |= 1 << pin; 
  } else {
    reg &= ~(1 << pin);
  }

  // write the new reg
  setRegister(addr, reg);

}

/**
 * Set the default values on port  to check against for interrupting (if set that way)
 * See setInterruptControl functions to set this behavior.
 */
void Adafruit_MCP23017::setDefaultValueA(uint8_t defbits ) {
  setRegister(MCP23017_DEFVALA, defbits);
}
void Adafruit_MCP23017::setDefaultValueB(uint8_t defbits ) {
   setRegister(MCP23017_DEFVALB, defbits);
}
/**
 * Set the default values on port A and B to check against for interrupting (if set that way)
 * See setInterruptControl functions to set this behavior.
 */
void Adafruit_MCP23017::setDefaultValueAB( uint16_t defbits ) {
   set2Registers(MCP23017_DEFVALA, defbits);
}


//-----------INTERRUPT: GETTING DEFAULT PIN VALUES-----------------------------
/**
 * Get the default value of the pin.  Port A pins [0:7},  Port B pins [8:15] 
 *   Returns a uint8_t with pin default value as LSB bit. 
 */
uint8_t Adafruit_MCP23017::getDefaultValuePin(uint8_t pin) {
  uint8_t reg;
  uint8_t addr;

  // only 16 bits!
  if (pin > 15)
    return 0;    //for lack of anything else right now

  if (pin < 8) {
    addr = MCP23017_DEFVALA;
  } else {
    addr = MCP23017_DEFVALB;
    pin -= 8;
  }

  // read the current DEFVALx
  reg = getRegister( addr );
  reg &= (1 << pin);       //mask off all others
  reg = reg >> pin;         //shift to LSB
   
  return reg;
}


uint8_t Adafruit_MCP23017::getDefaultValueA() {
    return getRegister(MCP23017_DEFVALA);
}
uint8_t Adafruit_MCP23017::getDefaultValueB() {
    return getRegister(MCP23017_DEFVALB);
}
/**
 * Assumes sequential operation.
 */
uint16_t Adafruit_MCP23017::getDefaultValueAB() {
    return get2Registers(MCP23017_DEFVALA);
}



//-----------INTERRUPT: GETTING INTERRUPT FLAGS-----------------------------
/**
 * See if a given pin was source of an interrupt.
 *   Port A pins [0:7},  Port B pins [8:15] 
 *   Returns a uint8_t with pin default value as LSB bit. 
 */
uint8_t Adafruit_MCP23017::getInterruptFlagPin(uint8_t pin) {
  uint8_t reg;
  uint8_t addr;

  // only 16 bits!
  if (pin > 15)
    return 0;

  if (pin < 8) {
    addr = MCP23017_INTFA;
  } else {
    addr = MCP23017_INTFB;
    pin -= 8;
  }

  // read the current INTF
  reg = getRegister( addr );
  reg &= (1 << pin);       //mask off all others
  reg = reg >> pin;         //shift to LSB
   
  return reg;
}

/**
 * Return the Interrupt Flag (who interrupted?) for port A
 */
uint8_t Adafruit_MCP23017::getInterruptFlagA() {
    return getRegister(MCP23017_INTFA);
}
uint8_t Adafruit_MCP23017::getInterruptFlagB() {
    return getRegister(MCP23017_INTFB);
}
uint16_t Adafruit_MCP23017::getInterruptFlagAB() {
    return get2Registers(MCP23017_INTFA);
}


//-----------INTERRUPT: GETTING INTERRUPT CAPTURE VALUES-----------------------------
/**
 * At time of interrupt the values of GPIO are copied into the INTCAP
 * register.  
 * IMPORTANT NOTE: The interrupt will remain active until the INTCAP or GPIO register is read.
 *  Note sure if this clear applies to both ports or each individually.
 */
uint8_t Adafruit_MCP23017::getInterruptCaptureA() {
    return getRegister(MCP23017_INTCAPA);
}
uint8_t Adafruit_MCP23017::getInterruptCaptureB() {
    return getRegister(MCP23017_INTCAPB);
}
uint16_t Adafruit_MCP23017::getInterruptCaptureAB() {
    return get2Registers(MCP23017_INTCAPA);
}

 //The interrupt will remain active until the INTCAP or GPIO register is read.




/********************************************************
* UTILITY and WORKER METHODS
*
* Note, a separate library of these functions can
* be found on mizraith's github site:   i2CHelper library
*********************************************************/
/**
 * Given a register (reg) and a bit position (bitpos [0-7])
 * and a bit value (0/1), left shift the bit value into
 * the register.  NOTE:   LSB is bit 0. MSB is bit 7.
 * Assumes user knows what they want and writes the
 * register out regardless--even if the same value as previous.
 */
void Adafruit_MCP23017::setBit(uint8_t reg, uint8_t bitpos, uint8_t bitval) {
	uint8_t current;
	current = getRegister( reg );
	
	if ( bitval == 0 ) {
	    current = current & ( 0xFF & (0 << bitpos) );
	} else {
	    current = current | (1 << bitpos);
	}
}

/**
 * Set a register of the chip to val
 */
void Adafruit_MCP23017::setRegister(uint8_t reg, uint8_t val) {
  if(reg > 0x15) {
  	return;   //no registers higher than that
  }
  Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
  wiresend(reg);	
  wiresend(val);
  Wire.endTransmission();
}

/** 
 * Sets 2 sequential registers starting with address of reg
 * Commonly used for setting AB register values
 * ASSUMES BANK=0 and sequential operation.
 */
void Adafruit_MCP23017::set2Registers(uint8_t reg, uint16_t val) {
  Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
  wiresend(reg);	
  wiresend(val & 0xFF);
  wiresend(val >> 8);
  Wire.endTransmission();
}


/**
 * Given a register (reg) and a bit position (bitpos [0-7])
 * return the bit value (0/1) as the LSB of a uint8_t 
 * NOTE:   LSB is bit 0. MSB is bit 7.
 */
uint8_t Adafruit_MCP23017::getBit(uint8_t reg, uint8_t bitpos) {
	uint8_t current;
	current = getRegister( reg );
	
	//mask off all the other bits
	current = current & ( 1 << bitpos);
	//put into LSB
	current = current >> bitpos; 
	return current; 
}

/**
 * Read a register of the chip
 */
uint8_t Adafruit_MCP23017::getRegister(uint8_t reg) {
  uint8_t val;
  
  if(reg > 0x15) {
  	return 0;   //no registers higher than that
  }
  Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
  wiresend(reg);	
  Wire.endTransmission();
  
  Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 1);
  val = wirerecv();
  return val;
}

/** 
 * Gets 2 sequential registers starting with address of reg
 * Commonly used for getting AB register values
 * ASSUMES BANK=0 and sequential operation.
 */
uint16_t Adafruit_MCP23017::get2Registers(uint8_t reg) {
  uint16_t ba = 0;
  uint8_t a;

  // read the current GPIO output latches
  Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
  wiresend(reg);	
  Wire.endTransmission();
  
  Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 2);
  a = wirerecv();
  ba = wirerecv();
  ba <<= 8;
  ba |= a;

  return ba;
}







