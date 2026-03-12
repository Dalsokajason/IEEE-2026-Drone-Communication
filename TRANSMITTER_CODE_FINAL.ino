//THIS IS THE FINAL CODE FOR THE ROBOT ESP32. FUNCTION: READ COLOR SENSOR, CONFIRM COLOR, TRANSMIT CONFIRMED COLOR OVER ESP-NOW TO THE DRONE ESP.
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <esp_now.h>
#include <WiFi.h>


Adafruit_TCS34725 tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

enum Color { RED, GREEN, BLUE, PURPLE, UNKNOWN };
int antennaID[] = {1, 3, 2, 4}; //our task order is button antenna, crater antenna, crank antenna, keypad antenna
const int confidenceValue = 5; // confidence value determines how many colors we must read consecutively in order to confirm the color.
Color colorRecord[confidenceValue]; // Creates an array of colors that allows us to take a record of the colors read by the sensor.
Color colorDetected;
String confirmedColor;
int cycle = 0; //Keeps track of how many colors we've read. Resets when it equals confidenceValue - 1
int antennaCycle = 0; //tracks how many antenna we've read starting from zero. Use antennaID[antennaCycle] to get the actual antenna number.
bool ENABLE = true;
bool timerActive = false;
unsigned long startTime = 0;
unsigned long runDuration = 120000; // duration in ms (2 minutes)

// RECEIVER DRONE ESP32 MAC Address: e0:8c:fe:5d:ca:6c
uint8_t broadcastAddress[] = {0xE0, 0x8C, 0xFE, 0x5D, 0xCA, 0x6C};

Color classify(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
  if (c < 50) return UNKNOWN;

  float rn = (float)r / c;
  float gn = (float)g / c;
  float bn = (float)b / c;

  // i put purple first to try to get it to recognize purple before blue
  //try changing the bn, rn & gn to identify purple better
  //BUT be careful with not getting it confused with blue  

  /*James say hello but patty kill sammy
  This work by bn gn and rn being the color divided by c which is the brightness
  so the threshold of purple is all being .3 since purple is very potent in all
  color next is the main color is higher then the other so for it to register as
  green gn has to be higher then bn and rn the same for the other colors
  it makes sense right green would have more green then blue and red
  and red would have more red then green and red
  ** I got rid of the +.10 for them since sometimes they might be similar */

  if (bn > 0.3 && rn > 0.3 && gn > 0.3) return PURPLE ;

  else if (gn > 0.35 && gn >= rn  && gn >= bn) return GREEN;

  else if (bn >= 0.35 && bn >= rn && bn >= gn) return BLUE;

  else if (rn > 0.35 && rn >= gn  && rn >= bn ) return RED;

  else return UNKNOWN;
  }

  const char* colorName(Color c) {
    switch (c) {
      case RED: return "RED";
      case GREEN: return "GREEN";
      case BLUE: return "BLUE";
      case PURPLE: return "PURPLE";
      default: return "UNKNOWN";
  }
}

boolean confirmColor(Color colorRecord[], Color detectedColor) {
  //If we have recorded the color the amount of times equal to the confidence value,
  //loop through the color record and check if all colors in the color record are
  //the same. If they are, then the color is confirmed.
  Color previousColor = detectedColor;
  //We cycle the amount of times as the confidenceValue i.e. length of the colorRecord.
  for(int i = 0; i < confidenceValue; i++){
    if((colorRecord[i] == previousColor) && (previousColor != UNKNOWN)){
      previousColor = colorRecord[i];
      //Are we at the end of the array?
      if(i == confidenceValue - 1){
        //Color confirmed.
        return true;
      }
    }
    else {
      return false;
    }
  }
  return false;
}

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
char color[32];
int antenna;
} struct_message;


// Create a struct_message called transmission
struct_message transmission;


esp_now_peer_info_t peerInfo;


// callback when data is sent
//void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void transmitData(String confirmedColor, int antennaID) {
  // Set values to send
  strcpy(transmission.color, confirmedColor.c_str());
  transmission.antenna = antennaID;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &transmission, sizeof(transmission));
  
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  }

  void setup() {
  Serial.begin(115200);
  delay(300);

  if (!tcs.begin()) {
    Serial.println("TCS34725 not found.");
    while (1) delay(10);
  }

  Serial.println("TCS34725 ready.");

  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }


  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  int start_time;
  while(!Serial.available()){
    start_time = millis();
  }
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  Color col = classify(r, g, b, c);
  
  float rn = (c ? (float)r / c : 0);
  float gn = (c ? (float)g / c : 0);
  float bn = (c ? (float)b / c : 0);

  //DEBUG COLOR RGB VALUES
  //Serial.print("Raw R:"); Serial.print(r);
  //Serial.print(" G:"); Serial.print(g);
  //Serial.print(" B:"); Serial.print(b);
  //Serial.print(" C:"); Serial.print(c);
  
  //Serial.print(" | rn="); Serial.print(rn, 3);
  //Serial.print(" gn="); Serial.print(gn, 3);
  //Serial.print(" bn="); Serial.print(bn, 3);
  
  //Serial.print(" -> ");
  //Serial.println(colorName(col));

  //Record the color detected
  colorRecord[cycle] = col;

  if(cycle == confidenceValue - 1){
    if(confirmColor(colorRecord, col)){
      confirmedColor = colorName(colorRecord[0]);
      Serial.print("CONFIRMED COLOR:"); Serial.println(confirmedColor);
      transmitData(confirmedColor, antennaID[antennaCycle]);
      
    }
    cycle = 0;
  }
  else{
    cycle++;
  }

  //DEGUB PRINT COLOR RECORD ARRAY
  //for(int i = 0; i < confidenceValue; i++){
  //  Serial.println(colorName(colorRecord[i]));
  //}

  //DEBUG TRACE CYCLE
  //Serial.println(cycle);
  
  delay(200);
  while(millis() - start_time > 120000){
    
  }
}


