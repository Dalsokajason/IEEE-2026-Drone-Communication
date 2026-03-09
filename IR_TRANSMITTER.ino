#include <IRremote.hpp>

constexpr uint8_t IR_LED_PIN = 2;   // change if needed
uint16_t address = 0xBB;
uint8_t antnum1 = 0x09;
uint8_t antnum2 = 0x3A;
uint8_t antnum3 = 0x5C;
uint8_t antnum4 = 0x6F;

void setup() {
  Serial.begin(115200);
  delay(500);

  // Start IR transmitter on selected pin
  IrSender.begin(IR_LED_PIN, ENABLE_LED_FEEDBACK);

  //uint16_t address = 0xBB; // NEC 16-bit address
  //uint8_t command  = 0x5C;   // 0x50 + 0x0C = 0x5C

  Serial.print("Sending NEC -> address 0x");
  Serial.print(address, HEX);
  Serial.print(" command 0x");
  Serial.println(antnum3, HEX);

  // 3rd param = repeats (0 = send once)
  for (int i = 0; i <= 9; i++)
  {
    IrSender.sendNEC(address, antnum3, 0);
    delay(2000);
  }
  Serial.println("Sent!");
}

void loop() {
}
