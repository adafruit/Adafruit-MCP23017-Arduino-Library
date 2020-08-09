#include <Wire.h>
#include "Adafruit_MCP23017.h"

// Sample for usage of two MCP23017 I/O expander with an ESP32
// public domain!

TwoWire myWire = TwoWire(0);

Adafruit_MCP23017 mcp;
  
void setup() {  

  myWire.setClock(1700000); // set frequency to 1.7mhz

  mcp.begin(&myWire); // use default address 0
}

void loop() {
}
