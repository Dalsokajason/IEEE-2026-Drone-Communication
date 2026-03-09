#include <esp_now.h>
#include <WiFi.h>


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
}
 
void loop() {


}
