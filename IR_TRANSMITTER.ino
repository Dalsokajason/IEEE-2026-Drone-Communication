#include <IRremote.hpp>
//this code will send the current values of the
constexpr uint8_t IR_LED_PIN = 2;   // change if needed
const uint16_t address = 0x00BB; //this is the address of Earth
const unit8_t AntennaCodes[4] = {0x00, 0x30, 0x50, 0x60};//this info can be found the guidelines
const char* AntennaNum[4] = {"ant1", "ant2", "ant3", "ant4"};


const unit8_t ColorCodes[4] = {0x09, 0x0A, 0x0C, 0x0F};
const char* ColorName[4] = {"red", "green", "blue", "purple"};


const int numofsends = 3;
const int gapsofcombos = 800;


unit8_t makeCommand(unit_t antenna, unit8_t color){
  return(antenna|color);
}
void sendCombo(uint8_t antennaIdx, uint8_t colorIdx) {
  uint8_t ant = antennaCodes[antennaIdx];
  uint8_t col = colorCodes[colorIdx];
  uint8_t cmd = makeCommand(ant, col);
 
  Serial.print("Sending NEC -> ");
  Serial.print(AntennaCodes, [antennaIdx]);
  Serial.print("+");
  Serial.print(ColorName, [antennaIdx]);
  Serial.print(" | address 0x");
  Serial.print(address, HEX);
  Serial.print(" command 0x");
  Serial.println(cmd, HEX);


  for (int r = 0; r < numofsends; r++){
    IrSender.sendNEC(address,cmd, 0);
    delay(200);//gap between the repeats
  }
}


void setup() {
  Serial.begin(115200);//sets the bit rate
  delay(100);


  // Start IR transmitter on selected pin
  IrSender.begin(IR_LED_PIN, ENABLE_LED_FEEDBACK);
 
  for (int i = 0; i <= 4; i++)
  {
    for int j = 0; c < 4; c++{
      sendCombo(a, c);
      delay(800);//gap between each combo
    }
  }
  Serial.println("Done sending all antenna/color combos!");
}
void loop() {//if we want it to loop forever move the for loops in here
}
