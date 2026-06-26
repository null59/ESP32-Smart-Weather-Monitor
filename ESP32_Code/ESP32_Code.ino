#include <WiFi.h>
#include "DHT.h"
#include <Firebase_ESP_Client.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_PASSWORD";

// Firebase credentials
#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_DATABASE_URL"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Sensor
const int signalPin = 27;
DHT dht(signalPin, DHT11);

// -------------------------------------------------------
void WiFiConnector() {
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

// -------------------------------------------------------
void FirebaseStart() {
  //Assign the API key and database URL
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);   // auto reconnect WiFi if needed

  // Sign up anonymously
  Serial.print("Signing up anonymously... ");
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Success");
  } else {
    Serial.printf("Failed, reason: %s\n", config.signer.signupError.message.c_str());
  }
}

// -------------------------------------------------------
void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);
  WiFiConnector();
  FirebaseStart();
}

// -------------------------------------------------------
void loop() {
  delay(2000);

  // Read sensor
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Check if readings are valid and dht11 dont give nan
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println("----------------------------------------------------");
  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(hum);

  // Create a JSON object
  FirebaseJson json;
  json.set("temperature", temp);
  json.set("humidity", hum);

  // Send to Firebase RTDB
  Serial.print("Pushing to Firebase... ");
  if (Firebase.RTDB.setJSON(&fbdo, "/value", &json)) {
    Serial.println("Success");
  } else {
    Serial.println("FAILED");
    Serial.printf("Error: %s\n", fbdo.errorReason().c_str());
  }

  Serial.println("----------------------------------------------------");
}