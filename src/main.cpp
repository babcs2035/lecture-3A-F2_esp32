
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "KOSEI-THINKBOOK"
#define WIFI_PASSWORD "password"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBQztBx7W8Qed4b_85LvgAeaVYEeAdaBwA"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://bdm-34-default-rtdb.firebaseio.com/"

#define DEVICE_ID "kero"
#define AD_PIN 33

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// Function to get current timestamp in the required format
void getCurrentTimestamp(char *buffer, size_t bufferSize)
{
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  strftime(buffer, bufferSize, "%Y%m%d-%H%M%S", timeinfo);
}

// Function to add data to Firebase
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
    Serial.println("Ampere value added successfully");
  }
  else
  {
    Serial.println("Failed to add ampere value");
    Serial.println(fbdo.errorReason());
  }

  // Add status value
  if (Firebase.RTDB.setBool(&fbdo, statusPath, statusValue))
  {
    Serial.println("Status value added successfully");
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
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize time with JST (UTC+9)
  configTime(9 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // Wait for time to be set
  while (!time(nullptr))
  {
    Serial.print(".");
    delay(1000);
  }
}

void loop()
{
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    uint16_t analog_adc = analogRead(AD_PIN);
    uint32_t analog_mv = analogReadMilliVolts(AD_PIN);
    addData(analog_mv, true);
  }
}
