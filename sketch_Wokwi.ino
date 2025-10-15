#define BLYNK_TEMPLATE_ID "TMPL27pX6_JS3"
#define BLYNK_TEMPLATE_NAME "Surveillance environnementale"
#define BLYNK_AUTH_TOKEN "PH51c_bjge_FFT-1NPxgZAWcBBmmIa22"
#define BLYNK_PRINT Serial



#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHTesp.h"  // Bibliothèque pour le capteur DHT22
#include <ESP32Servo.h>   // Bibliothèque pour le servo moteur compatible ESP32

// WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* pass = "";

// LED simple
const int ledPin = 14;

// Définir la broche du buzzer
const int buzzerPin = 19;  // Pin pour le buzzer

// Servo moteur
const int servoPin = 13;  // Pin pour le servo moteur
Servo servo;

// Capteur de mouvement (PIR)
const int pirPin = 12;  // Pin pour le capteur PIR
bool motionDetected = false;

// DHT22
const int DHT_PIN = 15;
DHTesp dht;

// Variables pour la température et l'humidité
float temperature = 0.0;
float humidity = 0.0;

// Variables pour la gestion des délais
unsigned long previousMillis = 0;
const long interval = 2000;  // Intervalle de lecture du capteur

void setup() {
  // Initialisation du port série
  Serial.begin(9600);

  // Configuration des pins
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);  // Initialiser le buzzer comme sortie

  // Initialisation du servo moteur
  servo.attach(servoPin);
  servo.write(0);  // Position initiale du servo

  // Initialisation du DHT
  dht.setup(DHT_PIN, DHTesp::DHT22);

  // Connexion au WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connexion au WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté au WiFi");

  // Connexion à Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connexion à Blynk...");
}

void loop() {
  // Lire les données de température et d'humidité toutes les 2 secondes
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();

    TempAndHumidity data = dht.getTempAndHumidity();

    if (!isnan(data.temperature) && !isnan(data.humidity)) {
      // Mettre à jour les variables
      temperature = data.temperature;
      humidity = data.humidity;

      // Afficher les données sur le port série
      Serial.print("Température: ");
      Serial.print(temperature);
      Serial.print("°C, Humidité: ");
      Serial.print(humidity);
      Serial.println("%");

      // Envoyer les données à l'application Blynk
      Blynk.virtualWrite(V1, temperature); // Température sur V1
      Blynk.virtualWrite(V2, humidity);    // Humidité sur V2
    } else {
      Serial.println("Erreur de lecture du capteur DHT!");
    }
  }

  // Lire l'état du capteur de mouvement
  motionDetected = digitalRead(pirPin) == HIGH;

  // Si un mouvement est détecté, afficher le message "Mouvement détecté" sur V3
  if (motionDetected) {
    Blynk.virtualWrite(V3, "Mouvement détecté !");
  } else {
    Blynk.virtualWrite(V3, "Pas de mouvement");
  }

    // Exécuter les fonctions Blynk
    Blynk.run();
}

// Fonction pour contrôler la LED simple via le bouton virtuel V0
BLYNK_WRITE(V0) {
  int pinValue = param.asInt();  // Récupère l'état du bouton (HIGH ou LOW)
  digitalWrite(ledPin, pinValue);  // Allume ou éteint la LED
}

// Fonction pour contrôler le servo moteur via un slider virtuel V4
BLYNK_WRITE(V4) {
  int angle = param.asInt();  // Récupère l'angle du slider (0-180)
  servo.write(angle);         // Déplace le servo à l'angle spécifié
}

// Fonction pour contrôler le buzzer via le bouton virtuel V5
BLYNK_WRITE(V5) {
  int buzzerState = param.asInt();  // Récupère l'état du bouton (HIGH ou LOW)
  
  if (buzzerState == 1) {
    tone(buzzerPin, 1000);  // Activer le buzzer à 1000 Hz
    Serial.println("Buzzer activé !");
  } else {
    noTone(buzzerPin);      // Désactiver le buzzer
    Serial.println("Buzzer désactivé !");
  }
}
