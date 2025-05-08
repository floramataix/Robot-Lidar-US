#include <SoftwareSerial.h>
#include <Servo.h>

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

const int trigPins[8] = {3, 4, 5, 6, 7, 8, 9, 10};  // Broches des capteurs (trig)
const int echoPins[8] = {11, 12, 13, A0, A1, A2, A3, A4};  // Broches Echo des capteurs

Servo micro_servo;
int pos = 0;

void setup() {
  // Initialisation des pins pour les capteurs
  for (int i = 0; i < 8; i++) {
    pinMode(trigPins[i], OUTPUT);  // Trig comme sortie
    pinMode(echoPins[i], INPUT);   // Echo comme entrée
  }

  pinMode(Led_pin, OUTPUT);
  Serial.begin(9600);
  BTSerial.begin(9600);  
  micro_servo.attach(7);  // Initialisation du servo

  Serial.println("Arduino avec HC-0x");
}

void loop() {
  delay(100);
  if (BTSerial.available()) {   // Lecture des commandes depuis l'application via Bluetooth  
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
    float distance = getDistance(trigPins[i], echoPins[i]);

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
