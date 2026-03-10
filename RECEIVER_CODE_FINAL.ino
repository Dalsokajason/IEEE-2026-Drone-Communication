#include <IRremote.hpp>
#include <esp_now.h>
#include <WiFi.h>

constexpr uint8_t IR_LED_PIN = 2;
uint8_t colorCodes[4] = {0x09, 0x0A, 0x0C, 0x0F}; //Red, Green, Blue, Purple
const char* colorNames[] = {"RED", "GREEN", "BLUE", "PURPLE"};
uint8_t antennaCodes[4] = {0x00, 0x30, 0x50, 0x60}; //1, 2, 3, 4
int antennaNames[] = {1, 2, 3, 4};
  uint16_t address = 0xBB; // NEC 16-bit address
uint8_t IRdata = 0;
uint8_t antennaData = 0;
uint8_t colorData = 0;

void grabData(const char* color, int antenna){
  for(int i = 0; i < 4; i++){
    if(strcmp(color, colorNames[i]) == 0){
      colorData = colorCodes[i];
    }
  }

  for(int j = 0; j < 4; j++){
    if(antenna == antennaNames[j]){
      antennaData = antennaCodes[j];
    }
  }

  IRdata = antennaData + colorData;
}
void sendIRData(uint8_t address, uint8_t IRdata) {
  for (int i = 0; i <= 9; i++) {
    IrSender.sendNEC(address, IRdata, 0);
    delay(2000);
  }
  Serial.print("Sending NEC -> address 0x");
  Serial.print(IRdata, HEX);
  Serial.println("RAHHH!!! Sent!");
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
  
  //grab the data from the transmission
  grabData(transmission.color, transmission.antenna);
}

void setup() {
  Serial.begin(115200);

  // Start IR transmitter on selected pin
  IrSender.begin(IR_LED_PIN);
 
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
}
void loop() {
  
  sendIRData(address, IRdata);
  delay(1000);

}
