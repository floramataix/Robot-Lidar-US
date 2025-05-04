
#include "Ultrasonic.h"
#include <Adafruit_NeoPixel.h>
//bibliothéque anneau de leds

/*
 * Pass as a parameter the trigger and echo pin, respectively,
 * or only the signal pin (for sensors 3 pins), like:
 * Ultrasonic ultrasonic(13);
 */
Ultrasonic ultrasonic(3,4);
double distance;

#define PIN 2 //sur quel pin de la carte arduino l'anneau de leds et connecté
#define NUMPIXELS 35  //nombre de leds sur l'anneau de leds

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  pixels.begin(); // initialise l'anneau de leds
}

void loop() {
  // Pass INC as a parameter to get the distance in inches
  distance = ultrasonic.read();
  Serial.print("Distance in cm: ");
  Serial.println(distance);


  for(int i=0; i<7; i++){
      pixels.setBrightness(50);
      if (tab[i]<100){
        for(int j=0; j<4; j++){
          pixels.setPixelColor(i+j, pixels.Color(150, 0, 0)); //s'allume en rouge quand la distance est inféreieur à 1 m
        }
      }

      else{
        for(int j=0; j<4; j++){
          pixels.setPixelColor(i+j, pixels.Color(0, 150, 0)); //s'allume en vert
        }
      }
    

      pixels.show();
      delay(10);
  }
}