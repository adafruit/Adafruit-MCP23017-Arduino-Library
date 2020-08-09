#include <Wire.h>
#include "Adafruit_MCP23017.h"

// Sample for usage of two MCP23017 I/O expander with an ESP32
// public domain!

Adafruit_MCP23017 firstMcp;
Adafruit_MCP23017 secondMcp;
  
void setup() {  

  Wire.begin(21, 22);
  Wire1.begin(4, 5);

  firstMcp.begin(&Wire); // use default address 0 for I2C #1
  secondMcp.begin(&Wire1); // use default address 0 for I2C #2
}

void loop() {
}
