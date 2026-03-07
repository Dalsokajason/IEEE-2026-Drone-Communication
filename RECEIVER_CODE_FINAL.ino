#include <esp_now.h>
#include <WiFi.h>
#include <IRremote.hpp>

//this code will send the current values of the
constexpr uint8_t IR_LED_PIN = 2;   // change if needed
const uint16_t address = 0x00BB; //this is the address of Earth
const uint8_t AntennaCodes[4] = {0x00, 0x30, 0x50, 0x60};//this info can be found the guidelines
const char* AntennaNum[4] = {"ant1", "ant2", "ant3", "ant4"};

const uint8_t ColorCodes[4] = {0x09, 0x0A, 0x0C, 0x0F};
const char* ColorName[4] = {"red", "green", "blue", "purple"};

const int numofsends = 3;
const int gapsofcombos = 800;

uint8_t makeCommand(wint_t antenna, uint8_t color){
  return(antenna|color);
}
void sendCombo(uint8_t antennaIdx, uint8_t colorIdx) {
  uint8_t ant = AntennaCodes[antennaIdx];
  uint8_t col = ColorCodes[colorIdx];
  uint8_t cmd = makeCommand(ant, col);
 
  Serial.print("Sending NEC -> ");
  Serial.print(AntennaCodes[antennaIdx]);
  Serial.print("+");
  Serial.print(ColorName[antennaIdx]);
  Serial.print(" | address 0x");
  Serial.print(address, HEX);
  Serial.print(" command 0x");
  Serial.println(cmd, HEX);


  for (int r = 0; r < numofsends; r++){
    IrSender.sendNEC(address,cmd, 0);
    delay(200);//gap between the repeats
  }
}


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    char color[32];
    int antenna;
} struct_message;


// Create a struct_message called transmission
struct_message transmission;


// callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&transmission, incomingData, sizeof(transmission));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Color Detected: ");
  Serial.println(transmission.color);
  Serial.print("Antenna: ");
  Serial.println(transmission.antenna);
  for(int i = 0; i < 4; i++){
    if(strcasecmp(ColorName[i], transmission.color) == 0){
        sendCombo(transmission.antenna, i);
    }
  }
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  delay(100);


  // Start IR transmitter on selected pin
  IrSender.begin(IR_LED_PIN, ENABLE_LED_FEEDBACK);
  
  

}

void loop() {//if we want it to loop forever move the for loops in here
}