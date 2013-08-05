/**************************************************************************
 * Example sketch using mizraith's expanded Adafruit_MCP23017.h library
 *
 * Basically blinks an LED on one of the 2 GPIO output ports.
 *
 * code available on github:  https://github.com/mizraith
 *
 * Released to public domain.  Enjoy!
 **************************************************************************/

#include <Wire.h>
#include "Adafruit_MCP23017.h"

/******************************************************************************
 DATA CONNECTIONS:   (you're on your own for power/ground connections) 
    Arduino A5  -->  SCL    (i2C clock)
    Arduino A4  -->  SDA    (i2C data)
    MCP23017 port A (pins 21:28)  [edit code if you want to hook them up here]
    MCP23017 port B (pins 1:8)    [Hook up LEDs with series resistors here]
    
    D13 - heartbeat blink
    
    Be sure to set the address bits (A2A1A0) on your chip to match addressing below
    
 *******************************************************************************/
#define HEARTBEAT 13 

// Select the pin [0:15] to blink inverse of HEARTBEAT.  
// 0-7 is port A. 8-15 is port B
#define BLINKYPIN 9     
 
Adafruit_MCP23017 mcp;

//-------------ADDRESSING--------------------------------------------
// You must bias address pins on device.  Nominal is to ground all 
// three of a 000.   Example below has 001 set
// (last 3 bits are A2 A1 A0)  
//uint8_t chipaddr = 0b00000000;
uint8_t chipaddr = 0b00000001;    


/***************************************************
 *   SETUP
 ***************************************************/
void setup() {
  Serial.begin(57600);
  Serial.println();
  Serial.print("Setting up MCP23017 at address: ");
  Serial.print( MCP23017_ADDRESS | chipaddr , BIN);
  Serial.println();
  
  mcp.begin(chipaddr);      // use default address 0

  uint8_t temp;
  
  //---------VERIFY IOCON SETUP ---------------
  temp = mcp.getRegister( MCP23017_IOCONA );
  Serial.print("IOCON A Register: ");
  Serial.print(temp, BIN);
  Serial.println();
  temp = mcp.getRegister( MCP23017_IOCONB );
  Serial.print("IOCON B Register: ");
  Serial.print(temp, BIN);
  Serial.print("   <-- They should be the same as they are shared");
  Serial.println();
  
  //---------CHECK DIRB prior to setup
  temp = mcp.getRegister( MCP23017_IODIRB );
  Serial.print("IODIRB Register initialized to: ");
  Serial.print(temp, BIN);
  Serial.println();
  
  //-----------SETUP OUR PORT DIRECTIONS -----------------  
//  mcp.setGPIOAMode(0x00);    // 1=input 0=output
//   mcp.setGPIOBMode(0x00);    //all outputs
    mcp.setGPIOABMode(0x0000);  //set them ALL to outputs for this demo

  //--------VERIFY PORT DIRECTION SETUP-------------------
  temp = mcp.getRegister( MCP23017_IODIRB );
  Serial.print("IODIRB Register now set to: ");
  Serial.print(temp, BIN);
  Serial.println();

  Serial.println("---------------------------------------------------");
  Serial.println("Commencing toggle of both heartbeat and blinky pin.");
}


/***************************************************
 *   MAIN LOOP
 ***************************************************/

//global counters outside of main loop()

void loop() {
  uint8_t temp = 0;
 
  delay(1000);
  
  digitalWrite(HEARTBEAT, !digitalRead(HEARTBEAT));
  mcp.writePin(BLINKYPIN, !digitalRead(HEARTBEAT));
  
  
  temp = mcp.readPin(BLINKYPIN);    //test our readback function
  
  Serial.print("BLINKYPIN: ");
  Serial.print(temp);
  Serial.println();
 
  
}
