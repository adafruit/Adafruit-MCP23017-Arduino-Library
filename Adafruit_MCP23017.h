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

#ifndef _Adafruit_MCP23017_H_
#define _Adafruit_MCP23017_H_

// Don't forget the Wire library
class Adafruit_MCP23017 {
  public:
	  void begin(uint8_t addr);
	  void begin(void);        
	  
	  //-----------------Pin Mode and Pull up Control-------------------
	  //When you see interface of (uint8_t, uint8_t) usually
	  //its a pin value [0:15] followed by a binary setting (HIGH/LOW)
	  void setPinMode(uint8_t p, uint8_t d);
	  void setGPIOAMode(uint8_t d);
	  void setGPIOBMode(uint8_t d);
	  void setGPIOABMode(uint16_t d);
	  
	  void setPinPullUp(uint8_t p, uint8_t d);
	  void setGPIOAPullUp(uint8_t d);
	  void setGPIOBPullUp(uint8_t d);
	  void setGPIOABPullUp(uint16_t d);
	  
	  //-----------------------Read and Write ---------------------------
	  void writePin(uint8_t p, uint8_t d);	
	  void writeGPIOA(uint8_t val);
	  void writeGPIOB(uint8_t val);
	  void writeGPIOAB(uint16_t);
	  
	  uint8_t readPin(uint8_t p);
	  uint8_t readGPIOA();
	  uint8_t readGPIOB();
	  uint16_t readGPIOAB();


      //----------------- INTERRUPT Functionality------------------------
      void disableInterrupts();
      void setInterruptMirror(uint8_t enable);  //1= OR INTA/INTB  0= no mirror   
      void setInterruptPolarity(uint8_t activehigh);  //1=activehigh 0=activelow
      
      void setInterruptPin(uint8_t pin, uint8_t enable );         //pin 0:15
      void setInterruptA(uint8_t enablemask);  //sets GPINTENx  1=enable int, 0=disable
      void setInterruptB(uint8_t enablemask);
      void setInterruptAB(uint16_t enablemask);
      
      // interrupts can be copmared against a (1=)default value or just a (0=)pin change
      void setInterruptControlPin(uint8_t pin, uint8_t enable ); //INTCONx  1=against defval  0=against previous
      void setInterruptControlA(uint8_t intconmask );
      void setInterruptControlB(uint8_t intconmask );
      void setInterruptControlAB( uint16_t intconmask );
      
      void setDefaultValuePin(uint8_t pin, uint8_t val);
      void setDefaultValueA(uint8_t defbits);   //sets DEFVALx
      void setDefaultValueB(uint8_t defbits);
      void setDefaultValueAB(uint16_t defbits);
      
      uint8_t getDefaultValuePin(uint8_t pin);
      uint8_t getDefaultValueA(); 
      uint8_t getDefaultValueB();
      uint16_t getDefaultValueAB();
      
      //Figure out what pin caused the interrupt.  "1" in the pin caused interrupt
      //The interrupt will remain active until the INTCAP or GPIO register is read.
      uint8_t getInterruptFlagPin(uint8_t pin);
      uint8_t getInterruptFlagA();
      uint8_t getInterruptFlagB();
      uint16_t getInterruptFlagAB();
      //Get the value of the port at the time the interrupt was captured.
      //These values are unchanged until a read of INTCAP or GPIO (writing to GPIO does not clear)
      //The interrupt will remain active until the INTCAP or GPIO register is read.
      //Until cleared, subsequent interrupts do not clock into intcap
      uint8_t getInterruptCaptureA();
      uint8_t getInterruptCaptureB();
      uint16_t getInterruptCaptureAB();

	  //----------------------Utility functions----------------------------
	  // These can be found on there own in my github i2CHelper library
	  //-------------------------------------------------------------------
	  void setBit(uint8_t reg, uint8_t bitpos, uint8_t bitval);
	  void setRegister(uint8_t reg, uint8_t val);
	  void set2Registers(uint8_t reg, uint16_t val);
	  uint8_t getBit(uint8_t reg, uint8_t bitpos);
	  uint8_t getRegister(uint8_t reg);
	  uint16_t get2Registers(uint8_t reg);


  private:
	  uint8_t i2caddr;
	  
};

#define MCP23017_ADDRESS 0x20   //Upper 4 bits of 7-bit address is 0b0100   A2A1A0 are lower 3 bits  

// registers when ---->IOCON.BANK = 0<-----------  
// This is most useful to reading/writing both parts at same time
// as teh addresses of IOA and IOB are sequential.
//IODIRx - when bit set, pin becomes input. Output when bit clear.
#define MCP23017_IODIRA 0x00
#define MCP23017_IODIRB 0x01

//IPOLx - when bit set, GPIO register bit will be inverted value of the pin
#define MCP23017_IPOLA 0x02
#define MCP23017_IPOLB 0x03

//GPINTENx - when bit set, pin is enabled for interrupt-on-change. 
//     Note: DEFVAL adn INTCON must also be set for the pin.
#define MCP23017_GPINTENA 0x04
#define MCP23017_GPINTENB 0x05

//DEFVALx - default comparison value, opposite value will cause an interrrup to occur
#define MCP23017_DEFVALA 0x06
#define MCP23017_DEFVALB 0x07

//INTCONx - when bit set, pin is compared against DEFVAL.  Bit cleared compares against previous value.
#define MCP23017_INTCONA 0x08
#define MCP23017_INTCONB 0x09

//IOCONx - Control and configuration. 
//According to datasheet, this is a "shared" register between A & B
#define MCP23017_IOCONA 0x0A
#define MCP23017_IOCONB 0x0B

//GPPUx - when bit set, pin has 100k pullup resistor enabled
#define MCP23017_GPPUA 0x0C
#define MCP23017_GPPUB 0x0D

//INTFx - a 'set' bit indicates that the associated pin caused the interrupt.  Red only register.
#define MCP23017_INTFA 0x0E
#define MCP23017_INTFB 0x0F

//INTCAPx - Captures GPIO at time of interrupt.  Read Only.  Remains unchanged until cleared via
//  a read of INTCAP or GPIO
#define MCP23017_INTCAPA 0x10
#define MCP23017_INTCAPB 0x11

//GPIOx - read/write.  Writing actually modifies output latch (OLAT) register.
#define MCP23017_GPIOA 0x12
#define MCP23017_GPIOB 0x13

//OLATx - output latch access.  read/write.  Write modifies the output latches that modifies the pins config'd as outputs.
#define MCP23017_OLATA 0x14
#define MCP23017_OLATB 0x15


// --------bit location definitions to enable shifting ---------------------------------------
// BANK - change how registers are mapped. USE CAUTION, as address changes after byte is clocked into the device
#define MCP23017_BIT_BANK 7
// MIRROR:  When bit set, INTn pins are OR'ed so int on either port causes both INT pins to activate.
#define MCP23017_BIT_MIRROR 6
//SEQOP - enable or disable sequential read. 1=disabled  0 = enabled(default)
// When enabled, address pointer auto-increments, otherwise remains fixed.
//     Useful to disable if continuously polling/modifying a register.
#define MCP23017_BIT_SEQOP 5   
//DISSLW - when bit set, slew rate is controlled on the SDA pin when driving from high to low
#define MCP23017_BIT_DISSLW 4
//HAEN - when disbabled, A2,A1,A0 are ignored (effectively being 000), but still must be externally biased for some reason.
#define MCP23017_BIT_HAEN 3
//ODR - enable/disable the INT pin for open-drain configuration.  Erasing this bit overrides INTPOL bit.
#define MCP23017_BIT_ODR 2
//INTPOL - sets polarity of INT pin to active push pull only when ODR is cleared.  1=Active High  0=Active Low
#define MCP23017_BIT_INTPOL 1



#endif
