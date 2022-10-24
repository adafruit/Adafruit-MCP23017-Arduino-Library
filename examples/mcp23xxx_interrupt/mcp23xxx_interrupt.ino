// NOTE: This is a simple example that only reads the INTA or INTB pin
//       state. No actual interrupts are used on the host microcontroller.
//       MCP23XXX supports the following interrupt modes:
//           * CHANGE - interrupt occurs if pin changes to opposite state
//           * LOW - interrupt occurs while pin state is LOW
//           * HIGH - interrupt occurs while pin state is HIGH

// ok to include only the one needed
// both included here to make things simple for example
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>

#define BUTTON_PIN 1   // MCP23XXX pin used for interrupt

#define INT_PIN 7      // microcontroller pin attached to INTA/B

// only used for SPI
#define CS_PIN 6

// uncomment appropriate line
Adafruit_MCP23X08 mcp;
//Adafruit_MCP23X17 mcp;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  Serial.println("MCP23xxx Interrupt Test!");

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C()) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("Error.");
    while (1);
  }

  // configure MCU pin that will read INTA/B state
  pinMode(INT_PIN, INPUT);

  // OPTIONAL - call this to override defaults
  // mirror INTA/B so only one wire required
  // active drive so INTA/B will not be floating
  // INTA/B will be signaled with a LOW
  mcp.setupInterrupts(true, false, LOW);

  // configure button pin for input with pull up
  mcp.pinMode(BUTTON_PIN, INPUT_PULLUP);

  // enable interrupt on button_pin
  mcp.setupInterruptPin(BUTTON_PIN, LOW);

  Serial.println("Looping...");
}

void loop() {
  if (!digitalRead(INT_PIN)) {
    Serial.print("Interrupt detected on pin: ");
    Serial.println(mcp.getLastInterruptPin());
    Serial.print("Pin states at time of interrupt: 0b");
    Serial.println(mcp.getCapturedInterrupt(), 2);
    delay(250);  // debounce
    // NOTE: If using DEFVAL, INT clears only if interrupt
    // condition does not exist.
    // See Fig 1-7 in datasheet.
    mcp.clearInterrupts();  // clear
  }
}