#include <SoftwareSerial.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include "Ultrasonic.h"

//define pour l'anneau de leds
#define PIN A7 //sur quel pin de la carte arduino l'anneau de leds et connecté
#define NUMPIXELS 35  //nombre de leds sur l'anneau de leds

// Définition des broches des capteurs
const int numCapteurs = 8;  // Nombre de capteurs
int triggerPins[numCapteurs] = {3,5,7,9,11,13,A3,A5};  // Broches Trigger
int echoPins[numCapteurs] = {2,4,6,8,10,12,A2,A4};     // Broches Echo

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define rxPin A9  // Broche 6 en tant que RX, à raccorder sur TX du HC-05
#define txPin A8  // Broche 7 en tant que TX, à raccorder sur RX du HC-05
#define Led_pin 2 //Broche de la LED
SoftwareSerial BTSerial(rxPin, txPin); // RX | TX


Servo micro_servo;

int scanDelay = 500;  // Valeur par défaut (500 ms)
char c = ' ';
String receivedWord = "";
int val = 0;
const int LIMITE_OBSTACLE = 10; // Distance seuil (en cm)
String positionsObstacle = "";


// Création d'un tableau de capteurs
Ultrasonic capteurs[numCapteurs] = {
  Ultrasonic(triggerPins[0], echoPins[0]),
  Ultrasonic(triggerPins[1], echoPins[1]),
  Ultrasonic(triggerPins[2], echoPins[2]),
  Ultrasonic(triggerPins[3], echoPins[3]),
  Ultrasonic(triggerPins[4], echoPins[4]),
  Ultrasonic(triggerPins[5], echoPins[5]),
  Ultrasonic(triggerPins[6], echoPins[6]),
  Ultrasonic(triggerPins[7], echoPins[7])
};

void setup() {
  Serial.begin(9600);// Pour envoyer des logs dans le Serial Monitor (Arduino IDE - via USB)
  Serial2.begin(9600);
  BTSerial.begin(9600);  
  micro_servo.attach(7);  // Initialisation du servo
  Serial.println("Arduino avec HC-0x");
  Serial.begin(9600);
  pixels.begin(); // initialise l'anneau de leds
  pixels.setBrightness(25);

}

void sendInfo(int tab[], int size){
  // Envoi du tableau : "2,5,0,0,56,78,12,99\n"
  for (int i = 0; i < numCapteurs; i++) {
    Serial2.print(tab[i]);
    if (i < numCapteurs - 1) Serial2.print(',');   // séparateur
  }
  Serial2.print('\n');  // fin de paquet
  
  delay(150);
}

void loop() {

  // Lecture des commandes depuis l'application via Bluetooth
  if (BTSerial.available()) {   
    c = BTSerial.read();  // Lecture du caractère envoyé
    receivedWord = receivedWord + c;

    if (c == '*') { // Fin de commande
      receivedWord.remove(receivedWord.length() - 1);  // Retirer le caractère '*' en fin de commande

      if (receivedWord == "f") {  // Commande ON
        digitalWrite(Led_pin, LOW);
      } else if (receivedWord == "v") {  // Commande OFF
        digitalWrite(Led_pin, HIGH);
      } else {
        // Traitement pour ajuster le scanDelay
        val = 0;
        for (int i = 0; i < receivedWord.length(); i++) {
          if (isdigit(receivedWord[i])) {
            val = val * 10 + (receivedWord[i] - '0');
          }
        }
        scanDelay = val;
        Serial.print("Nouveau scanDelay : ");
        Serial.println(scanDelay);
      }

      receivedWord = "";
      while (BTSerial.available()) BTSerial.read();  // Vider le buffer Bluetooth
    }
  
  }
  int distances[numCapteurs] ;
  for (int i = 0; i < numCapteurs; i++) {
    distances[i] = capteurs[i].read();
  }

  for (int i = 0; i< numCapteurs; i++) {
    Serial.print("Capteur ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(distances[i]);
    Serial.println(" cm");
  }
  
    for(int i=0; i<8; i++){
      pixels.setBrightness(10);
      if (distances[i]<100){
        for(int j=0; j<4; j++){
          pixels.setPixelColor(i*4+j, pixels.Color(150, 0, 0)); //s'allume en rouge quand la distance est inféreieur à 1 m
        }
      }

      else{
        for(int j=0; j<4; j++){
          pixels.setPixelColor(i*4+j, pixels.Color(0, 150, 0)); //s'allume en vert
        }
      }
    

      
  }
  pixels.show();
  delay(10);
  
  delay(2500);
  Serial.println("===========");
  


  sendInfo(distances, numCapteurs);

}