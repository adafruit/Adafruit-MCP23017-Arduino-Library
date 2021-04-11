#include <Wire.h>
#include "Adafruit_MCP23017.h"

/*
   Example code for reading an 8x8 keypad matrix using the MCP23017 I2c Port Expander
   This example is not a complete example as it does not include
    - debouncing
    - key press events

    Rows are ouputs at the Arduino and connected to MCP23017 Port A
    Columns are inputs at the Arduino and connected to MCP23017 Port B
    
    Written by Richard Teel for TeelSys LLC.
*/

#define scantime 100

Adafruit_MCP23017 mcp;

unsigned long previousScanMillis = 0;

void setup() {
  mcp.begin();      // use default address 0
  // Set port direction
  mcp.portMode(MCP23017_PORT_A, OUTPUT);
  mcp.portMode(MCP23017_PORT_B, INPUT_PULLUP);
  mcp.portPolarity(MCP23017_PORT_B, true);
  mcp.writeGPIO(MCP23017_PORT_A, 0xff); // write 0xff to Port A

  Serial.begin(115200);   // PC for debuging
}

void loop() {
  unsigned long currentScanMillis = millis();

  if (currentScanMillis - previousScanMillis >= scantime) {
    previousScanMillis = currentScanMillis;

    uint8_t keyPressed = 0;

    for (uint8_t row = 0; row < 8; row++) {
      uint8_t q = ~(0x01 << row);
      mcp.writeGPIO(MCP23017_PORT_A, ~(0x01 << row)); // write rowScan to Port A to set row to 0 (low)

      uint8_t scanValue = mcp.readGPIO(MCP23017_PORT_B);

      // Check if a key was pressed in the row
      if (scanValue != 0x00) {
        uint8_t scanCode = 0;

        for (uint8_t col = 0; col < 8; col++) {
          // Was the key in the column pressed?
          if ((scanValue >> col) & 0x01 == 0x01) {
            keyPressed = 1;
            scanCode = (row * 8) + (col + 1);
            Serial.print("SCAN CODE = "); Serial.print(scanCode); Serial.print(", ROW = "); Serial.print(row); Serial.print(", COL = "); Serial.println(col);
          }
        }
      }
    }
    // Prints line to group multiple key presses together
    if (keyPressed) {
      Serial.println("---");
    }
  }
}