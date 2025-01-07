#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "credentials.h"

// Firebase object
FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

unsigned long sendDataPrevT = 0;
bool firebaseReadyFlag = false;

// Function to get current timestamp in the required format
void getCurrentTimestamp(char *buffer, size_t bufferSize)
{
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  strftime(buffer, bufferSize, "%Y%m%d-%H%M%S", timeinfo);
}

// Add data to Firebase
void addData(float ampereValue, bool statusValue)
{
  char timestamp[20];
  getCurrentTimestamp(timestamp, sizeof(timestamp));

  // Construct the paths for ampere and status
  char amperePath[100];
  char statusPath[100];
  snprintf(amperePath, sizeof(amperePath), "devices/%s/ampere/%s", DEVICE_ID, timestamp);
  snprintf(statusPath, sizeof(statusPath), "devices/%s/status/%s", DEVICE_ID, timestamp);

  // Add ampere value
  if (Firebase.RTDB.setFloat(&fbdo, amperePath, ampereValue))
  {
    Serial.printf("Ampere value added successfully: %f\n", ampereValue);
  }
  else
  {
    Serial.println("Failed to add ampere value");
    Serial.println(fbdo.errorReason());
  }

  // Add status value
  if (Firebase.RTDB.setBool(&fbdo, statusPath, statusValue))
  {
    Serial.printf("Status value added successfully: %d\n", statusValue);
  }
  else
  {
    Serial.println("Failed to add status value");
    Serial.println(fbdo.errorReason());
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\n--------------------------------\n");
  Serial.printf("Device: %s\n", DEVICE_ID);
  Serial.printf("AD_PIN: %d\n", AD_PIN);
  Serial.println("");

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.print("\nConnected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  // Connect to Firebase
  config.api_key = FIREBASE_API_KEY;
  config.database_url = FIREBASE_DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Connected to Firebase\n");
    firebaseReadyFlag = true;
  }
  else
  {
    Serial.printf("Failed to connect to Firebase: %s\n\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize time with JST (UTC+9)
  configTime(9 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr))
  {
    Serial.print(".");
    delay(1000);
  }
  sendDataPrevT = millis();
}

void loop()
{
  if (Firebase.ready() && firebaseReadyFlag && millis() - sendDataPrevT > 15000)
  {
    sendDataPrevT = millis();

    uint16_t analog_adc = analogRead(AD_PIN);
    uint32_t analog_mv = analogReadMilliVolts(AD_PIN);
    addData(analog_mv, true);
  }
}
