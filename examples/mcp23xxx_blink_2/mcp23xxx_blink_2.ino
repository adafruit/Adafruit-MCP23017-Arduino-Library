#include <Wire.h>
#include <Adafruit_MCP23X17.h>

// 创建 MCP23017 对象
//Create MCP23017 object
Adafruit_MCP23X17 mcp;

// 定义 LED 连接到 MCP23017 的引脚
// Define the pins where the LED is connected to the MCP23017
const int ledPinOnMCP23017 = 9;  // 替换为实际连接的引脚号 // Replace with the actual connected pin number

void setup() {
  // 初始化 MCP23017
  //Initialize MCP23017
  mcp.begin_I2C();  // 默认 I2C 地址为 0x20 //The default I2C address is 0x20

  // 配置引脚为输出 // Configure pin as output
  mcp.pinMode(ledPinOnMCP23017, OUTPUT);
}

void loop() {
  // 点亮 LED
  // open led
  mcp.digitalWrite(ledPinOnMCP23017, HIGH);
  delay(2000); // 等待 2 秒 //wait 2s

  // 关闭 LED
  // close led
  mcp.digitalWrite(ledPinOnMCP23017, LOW);
  delay(2000); // 等待 2 秒 //wait 2s
}
