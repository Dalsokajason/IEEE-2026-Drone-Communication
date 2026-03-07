//DO NOT USE, This code has been merged with TransmissionCode.ino Final version can be found at TransmissionCodeFinal.ino
//check the serial monitor for the color it identifies use the same BAND number!!
//you need to download the library adafruit from the arduino ide
#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

enum Color { RED, GREEN, BLUE, PURPLE, UNKNOWN };
const int confidenceValue = 5;
Color colorRecord[confidenceValue];
Color colorDetected;
int cycle = 0;

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

void setup() {
  Serial.begin(115200);
  delay(300);

  if (!tcs.begin()) {
    Serial.println("TCS34725 not found.");
    while (1) delay(10);
  }
  
  Serial.println("TCS34725 ready.");
}

void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  Color col = classify(r, g, b, c);
  
  float rn = (c ? (float)r / c : 0); 
  float gn = (c ? (float)g / c : 0);
  float bn = (c ? (float)b / c : 0);

  Serial.print("Raw R:"); Serial.print(r);
  Serial.print(" G:"); Serial.print(g);
  Serial.print(" B:"); Serial.print(b);
  Serial.print(" C:"); Serial.print(c);

  Serial.print(" | rn="); Serial.print(rn, 3);
  Serial.print(" gn="); Serial.print(gn, 3);
  Serial.print(" bn="); Serial.print(bn, 3);

  Serial.print(" -> ");
  Serial.println(colorName(col));

  //Record the color detected
  colorRecord[cycle] = col;

  if(cycle == confidenceValue - 1){
    Serial.println("Cycle has reached 4");
    if(confirmColor(colorRecord, col)){
      Serial.print("CONFIRMED COLOR:"); Serial.println(colorName(colorRecord[0]));
    }
    cycle = 0;
  }
  else{
    cycle++;
  }

  for(int i = 0; i < confidenceValue; i++){
    Serial.println(colorName(colorRecord[i]));
  }

  Serial.println(cycle);
  
  delay(200);
}
