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

bool firebaseReadyFlag = false;
int collectedDataCnt, mv_max, mv_min;

// Function to get current timestamp in the required format
void getCurrentTimestamp(char *buffer, size_t bufferSize)
{
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  strftime(buffer, bufferSize, "%Y%m%d-%H%M%S", timeinfo);
}

// Add data to Firebase
void addData(float ampValue)
{
  char timestamp[20];
  getCurrentTimestamp(timestamp, sizeof(timestamp));

  // Construct the paths for ampere and status
  char ampPath[100];
  snprintf(ampPath, sizeof(ampPath), "devices/%s/amp/%s", DEVICE_ID, timestamp);

  // Add ampere value
  if (Firebase.RTDB.setFloat(&fbdo, ampPath, ampValue))
  {
    Serial.printf("Amp value added successfully: %f\n", ampValue);
  }
  else
  {
    Serial.println("Failed to add amp value");
    Serial.println(fbdo.errorReason());
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(AD_PIN, INPUT);
  analogSetPinAttenuation(AD_PIN, ADC_11db);
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

  collectedDataCnt = 0;
  mv_max = 0;
  mv_min = 100000;
}

void loop()
{
  collectedDataCnt += 1;
  uint32_t analog_mv = analogReadMilliVolts(AD_PIN);
  if (mv_max < analog_mv)
  {
    mv_max = analog_mv;
  }
  if (mv_min > analog_mv)
  {
    mv_min = analog_mv;
  }

  if (collectedDataCnt % 1000 == 0)
  {
    int amplitude = (mv_max - mv_min) / 2;
    // Serial.printf("%d,%4d,%4d,%4d\n", n, mv_max, mv_min, amplitude);
    if (Firebase.ready() && firebaseReadyFlag)
    {
      addData(amplitude);
    }

    mv_max = 0;
    mv_min = 100000;
  }

  delay(1);
}
