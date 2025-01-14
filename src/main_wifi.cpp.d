#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "babcs-iPhone"; //  *** 書き換え必要 ***
const char *password = "password"; //  *** 書き換え必要（8文字以上）***

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("connecting");
  }
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  WiFiClient client = server.available();
  if (client)
  { // 接続PCが存在
    while (client.connected())
    { // PCがつながっている間、以下をループ
      client.println("<!DOCTYPE html><html lang='ja'>");
      client.println("<head><meta charset='UTF-8'>");
      client.println("<title>kero</title>");
      client.println("</head><body><h1>kero</h1></body></html>");
      Serial.println("client connected");
      delay(1000);
    }
    client.stop();
  }
}
