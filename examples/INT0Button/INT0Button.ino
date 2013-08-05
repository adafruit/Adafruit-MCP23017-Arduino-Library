/**************************************************************************
 * Example sketch using mizraith's expanded Adafruit_MCP23017.h library
 *
 * Basically looks at a button and on interrupt it toggles the state
 * of the onboard D13 LED (Arduino nanos).
 * This demosntrates a bit about how to set up the MCP23017's awesome
 * interrupt functionality....read .h file for additional info and 
 * spec sheet for details.  Note that the MCP doesn't have a rising
 * or falling edge interrupt.  This means that you would have to
 * add some logic or additional timers to get the desired button
 * driven interface.  This example instead just slows things down
 * (and allows time for user to depress button) by increasing the
 * DEBOUNCE_DELAY variable.  A better way would be to add logic
 * that checks the specific pin and looks for a depress or after
 * a given time allows the interrupt to trigger again (scan mode).
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
    MCP23017 port B (pins 1:8)    [Hook up a button anywhere to this port and ground]
    
    MCP23017 INTA or INTB output pins to Arduino INT0 (D2) pin
    
    D13 - onboard LED (Arduino Nano).
    
    Be sure to set the address bits (A2A1A0) on your chip to match addressing below
    
 *******************************************************************************/
#define LED_PIN 13 
#define INT_PIN 2
 
Adafruit_MCP23017 mcp;

//-------------ADDRESSING--------------------------------------------
// You must bias address pins on device.  Nominal is to ground all 
// three of a 000.   Example below has 001 set, so A0 is biased to VCC.
// (last 3 bits are A2 A1 A0)  
//uint8_t chipaddr = 0b00000000;
uint8_t chipaddr = 0b00000001;    


//-------------INTERRUPT AND OTHER GLOBALS--------------------------------------------
//track interrupt time for "debouncing" purposes
volatile unsigned long int_time = 0;        

//flag telling main loop to report on interrupt
volatile boolean serviceint = false;        

//In my case (bad button, no RC debounce), I find I cannot push button faster than 100ms anyway.
#define DEBOUNCE_DELAY 200

// storage for recording interrupt conditions
int16_t intflagAB=0, intcapAB=0, valAB=0, btncnt=0;   

//time (ms) between valid button presses 
unsigned long last_int_time = 0;




/***************************************************
 *   SETUP
 ***************************************************/
void setup() {
  uint8_t temp;
  uint16_t tempwide;
  
  Serial.begin(57600);
  pinMode(INT_PIN, INPUT);
//  digitalWrite(INT_PIN, HIGH);  //set pullup -- not necessary in this case. Hi-Z is better
  
  
  //---------START MCP COMMUNICATIONS ---------------
  Serial.println();
  Serial.print("Setting up MCP23017 at address: ");
  Serial.print( MCP23017_ADDRESS | chipaddr , BIN);
  Serial.println();
  
  mcp.begin(chipaddr);      // use default address 0

  
  //---------VERIFY IOCON SETUP ---------------
  temp = mcp.getRegister( MCP23017_IOCONA );
  Serial.print("IOCON A: ");
  Serial.print(temp, BIN);
  Serial.println();
  temp = mcp.getRegister( MCP23017_IOCONB );
  Serial.print("IOCON B: ");
  Serial.print(temp, BIN);
  Serial.print("   <-- They should be the same as they are shared");
  Serial.println();
  
  
  //-----------SETUP OUR PORT DIRECTIONS -----------------  
//  mcp.setGPIOAMode(0xFF);    // 1=input 0=output
//  mcp.setGPIOBMode(0xFF);    //all INPUTS
  mcp.setGPIOABMode(0xFFFF);   //all INPUTS on both ports
  

  //--------VERIFY PORT DIRECTION SETUP-------------------
  temp = mcp.getRegister( MCP23017_IODIRB );
  Serial.print("IODIRB: ");
  Serial.print(temp, BIN);
  Serial.println();


  //--------ENABLE PULLUPS  (default is off)-------------------
  mcp.setGPIOABPullUp(0xFFFF);    //all enabled
  temp = mcp.getRegister( MCP23017_GPPUB );
  Serial.print("GPPUB: ");
  Serial.print(temp, BIN);      //0 = pull up disabled, 1=pull up enabled
  Serial.println();
  
  
  //-------ENABLE ARDUINO INT0 interrupt routines------------------
  //-------do this before we start expecting to see interrupts-----
  //--------INT 0---------
  EICRA = 0;               //clear it
  EICRA |= (1 << ISC01);
  EICRA |= (1 << ISC00);   //ISC0[1:0] = 0b11  rising edge INT0 creates interrupt
  EIMSK |= (1 << INT0);    //enable INT0 interrupt
  // Note:  instead of above, could use Arduino version of same
  // attachInterrupt(0,  functionname , RISING);

  
  //--------ENABLE MCP INTERRUPTS  (default is off)-------------------
  mcp.setInterruptMirror( 1 );       //enable interrupt mirroring, so either port works
  mcp.setInterruptPolarity ( 1 );    // 1= active high
//  mcp.setInterruptControlAB( 0x0000);   //all pins interrupt on change only. Would catch button press & depress
  mcp.setInterruptControlAB( 0xFFFF ); //all pins compared against a default value. interrupt on mismatch
  mcp.setDefaultValueAB ( 0xFFFF);    //set default val for all pins to 1, since pullups enabled.
  //finally
  mcp.setInterruptAB( 0xFFFF );      //enable interrupt on both ports for all pins

  tempwide = mcp.get2Registers(MCP23017_IOCONA);
  Serial.println("-----MCP Interrupt Setup Follows-----");
  Serial.print("IOCONAB:  ");
  Serial.print(tempwide, BIN);
  Serial.println();
  Serial.print("Interrupt Control AB:    ");
  tempwide = mcp.get2Registers(MCP23017_INTCONA);
  Serial.print(tempwide, BIN);
  Serial.println();
  Serial.print("Default Values AB:       ");
  tempwide = mcp.get2Registers(MCP23017_DEFVALA);
  Serial.print(tempwide, BIN);
  Serial.println();
  Serial.print("Interrupt Enabled Pins:  ");
  tempwide = mcp.get2Registers(MCP23017_GPINTENA);
  Serial.print(tempwide, BIN);
  Serial.println();
  Serial.println();
  Serial.println("Entering loop...every time an interrupt occurs");
  Serial.println("The LED will toggle and the interrupt info will print out.");
  Serial.println();
  
  temp = mcp.readGPIOAB();    //This will clear interrupts on MCP prior to entering main loop.
  
}



/***************************************************
 *   ARDUINO INTERRUPT LOGIC
 ***************************************************/

/**
 * It's important to keep this method TIGHT.  
 * We set a flag and move on, as doing anything real
 * (like Serial.print) can cause problems..many of
 * those commands use interrupts themselves.
 */
ISR(INT0_vect) {
//    EIFR = 0;
    if(serviceint) {
      return;                             //the last int has not been serviced yet, could be a bounce
    } else {
      int_time = millis();
      serviceint = true;                  //set flag to service
      
//      sei();                            //have to re-enable other interrupts for below Serial.print to work
//      Serial.println("Inside ISR0");    //for debugging only
    }
}




/***************************************************
 *   MAIN LOOP
 ***************************************************/


void loop() {
    //why poll the status of a register, let the interrupts handle it all!

    if(serviceint) {
      handleINT();
    } 
    else {
      if(digitalRead(INT_PIN)) {
        //somehow our pin is high, but serviceint is false. Clear the int
        valAB = mcp.readGPIOAB();
      } 
    }
    
}

/**
 * Do all the 'heavy' lifting resulting from our interrupt.  This function
 * should be ready to be called frequently to aid in responsiveness.  
 * One reason for this method is that many of the calls (i2C adn Serial.print)
 * use interrupts themselves and should really not be called inside
 * the interrupt vector function.
 */
void handleINT() {
    unsigned long temp = int_time - last_int_time;
    Serial.print("Button press time: ");
    Serial.print(temp, DEC);
    Serial.println();

    if(temp > DEBOUNCE_DELAY) {
      btncnt++;
      last_int_time = int_time;
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));      //TOGGLE:  ^ 1);
      
      //i2c calls are brutal inside an interrupt vector, so we save those for here
      intflagAB = mcp.getInterruptFlagAB();     
      
      //NOTE: These next 2 will clear the interrupt.  The problem is that if the user
      //has _not_ released the button yet, it will simply re-interrupt again.  We
      //were seeing this every 30-60ms (about the time it takes for the user to
      //depress the button.  The solution is to put in a DELAY here.  This way,
      //if the user holds down the button it can continue to interrupt (as they
      //may intend) but at a reasonable rate.
      //I found a quick depress to be 30ms and a slow depress to be about 70ms.   
      intcapAB = mcp.getInterruptCaptureAB();    //this will clear the MCP interrupt
      valAB = mcp.readGPIOAB();                  //this will clear the MCP interrupt
      
      Serial.print("---GOOD:  "); 
      Serial.print(btncnt, DEC);
      Serial.println();
      Serial.println("Interrupt Info:");
      Serial.print("INTFLAG: ");
      Serial.print(intflagAB, BIN);
      Serial.println();
      Serial.print("INTCAP: ");
      Serial.print(intcapAB, BIN);
      Serial.println();
      Serial.print("GPIOAB: ");
      Serial.print(valAB, BIN);
      Serial.println();
      Serial.println();
    } else {
      Serial.print("-BOUNCE: ");
      Serial.print(btncnt, DEC);
      Serial.println();
      Serial.println();

      valAB = mcp.readGPIOAB();  //this will clear the MCP interrupt

    }
    
    delay(DEBOUNCE_DELAY);       //hack: Allow the user time to depress the button, or the MCP will interrupt again
    serviceint = false;

    
}
