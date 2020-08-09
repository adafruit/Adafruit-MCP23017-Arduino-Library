#include <Wire.h>
#include "Adafruit_MCP23017.h"

// Sample for usage of two MCP23017 I/O expander with an ESP32
// public domain!

TwoWire firstWire = TwoWire(0);
TwoWire secondWire = TwoWire(1);

Adafruit_MCP23017 firstMcp;
Adafruit_MCP23017 secondMcp;
  
void setup() {  

  firstWire.begin(21, 22);
  secondWire.begin(19, 23);

  firstMcp.begin(&firstWire); // use default address 0 for I2C #1
  secondMcp.begin(&secondWire); // use default address 0 for I2C #2
}

void loop() {
}
