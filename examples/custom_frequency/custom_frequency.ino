#include <Wire.h>
#include "Adafruit_MCP23017.h"

// Sample for usage of two MCP23017 I/O expander with an ESP32
// public domain!

Adafruit_MCP23017 mcp;
  
void setup() {  

  Wire.setClock(1700000); // set frequency to 1.7mhz

  mcp.begin(&Wire); // use default address 0
}

void loop() {
}
