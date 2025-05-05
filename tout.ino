#include <SoftwareSerial.h>
#include "Ultrasonic.h"
#include <Servo.h>
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

#define rxPin 6   // Broche 6 en tant que RX, à raccorder sur TX du HC-05
#define txPin 7   // Broche 7 en tant que TX, à raccorder sur RX du HC-05
#define Led_pin 2 //Broche de la LED
SoftwareSerial BTSerial(rxPin, txPin); // RX | TX

int scanDelay = 500;  // Valeur par défaut (500 ms)
char c = ' ';
String receivedWord = "";
int val = 0;
const float LIMITE_OBSTACLE = 100.0; // Distance seuil (en cm)
String positionsObstacle = "";


Servo micro_servo;
int pos = 0;

const int numCapteurs = 8;  // Nombre de capteurs
int triggerPins[numCapteurs] = {3,5,7,9,11,13,A3,A5};  // Broches Trigger
int echoPins[numCapteurs] = {2,4,6,8,10,12,A2,A4};     // Broches Echo

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
void sendInfo(int distances[], size_t size){
  Serial2.write((byte*)distances, size);
};





void setup() {
  // Initialisation des pins pour les capteurs
  for (int i = 0; i < 8; i++) {
    pinMode(triggerPins[i], OUTPUT);  // Trig comme sortie
    pinMode(echoPins[i], INPUT);   // Echo comme entrée
  }

  pinMode(Led_pin, OUTPUT);
  Serial.begin(9600);
  BTSerial.begin(9600);  
  micro_servo.attach(7);  // Initialisation du servo

  Serial.println("Arduino avec HC-0x");
  Serial1.begin(9600);   // Pour envoyer des logs dans le Serial Monitor (Arduino IDE - via USB)
  Serial2.begin(9600);  // Pour envoyer des informations à l'autre arduino (TX2 port 16 ; RX2 port 17)

  pixels.begin(); // initialise l'anneau de leds
}



// Compteur a utiliser a la place de delay() pour eviter de bloauer
int interval = 1000;
// Statique car on doit se rapeler a chaque tours de loop
static int dernierTemps = 0;

static int distances[numCapteurs];


void loop() {
  delay(100);

  for (int i = 0; i < numCapteurs; i++) {
    distances[i] = capteurs[i].read();
  }

  size_t sizedistances = numCapteurs;

  if (millis() - dernierTemps >= interval)
  {
    // M-a-j du temps actulle
    dernierTemps = millis();

    for (int i = 0; i< sizedistances; i++) {
      Serial.print("Capteur ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(distances[i]);
      Serial.println(" cm");
    } 
  }
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
  }



  sendInfo(distances, sizedistances);
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

  // Lire la distance de chaque capteur
  positionsObstacle = "";
  for (int i = 0; i < 8; i++) {
    float distance = capteurs[i].read();


    if (distance < LIMITE_OBSTACLE) {
      positionsObstacle += char('1');  
    }
    else{
      positionsObstacle += char('0');
    }
  }

  if (positionsObstacle.length() > 0) {
    BTSerial.println(positionsObstacle); 
  }

  delay(scanDelay);  // Attente avant le prochain scan
}

// Fonction pour obtenir la distance à un capteur
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);  // Mesure du temps
  return (duration * 0.0343) / 2;  // Calcul de la distance (en cm)
}






