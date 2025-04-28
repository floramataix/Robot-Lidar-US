#include "Ultrasonic.h"

// Définition des broches des capteurs
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
  Serial.begin(9600);
  Serial1.begin(9600);   // Pour envoyer des logs dans le Serial Monitor (Arduino IDE - via USB)
  Serial2.begin(9600);  // Pour envoyer des informations à l'autre arduino (TX2 port 16 ; RX2 port 17)
}

// Compteur a utiliser a la place de delay() pour eviter de bloauer
int interval = 1000;
// Statique car on doit se rapeler a chaque tours de loop
static int dernierTemps = 0;


void loop() {
  int distances[numCapteurs] ;
  for (int i = 0; i < numCapteurs; i++) {
    distances[i] = capteurs[i].read();
  }


  /*for (int i = 0; i < numCapteurs; i++) {
    delay(50);
    int distance = capteurs[i].read();
    Serial.print("Capteur ");
    Serial.print(i + 1 : );
    Serial.print(distance);
    Serial.println(" cm");
  }
  Serial.println("-------------------");*/
 
  size_t sizedistances = numCapteurs;

  if (millis() - dernierTemps >= interval)
  {
    // M-a-j du temps actulle
    nextTime += interval;

    for (int i = 0; i< sizedistances; i++) {
      Serial.print("Capteur ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(distances[i]);
      Serial.println(" cm");
    } 
  }
  
  sendInfo(distances, sizedistances);
}