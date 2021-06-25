// Reads a button attached to a MCP23XXX pin.

// ok to include only the one needed
// both included here to make things simple for example
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>

#define BUTTON_PIN 1  // MCP23XXX pin button is attached to

// only used for SPI
#define CS_PIN 6

// uncomment appropriate line
Adafruit_MCP23X08 mcp;
//Adafruit_MCP23X17 mcp;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  Serial.println("MCP23xxx Button Test!");

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C()) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("Error.");
    while (1);
  }

  // configure pin for input with pull up
  mcp.pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("Looping...");
}

void loop() {
  // LOW = pressed, HIGH = not pressed
  if (!mcp.digitalRead(BUTTON_PIN)) {
    Serial.println("Button Pressed!");
    delay(250);
  }
}