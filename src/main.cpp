#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <secrets.h>
#include <led.h>
#include <web.h>

const char *hostName = "lamp";

void startMDNS();
void setupWiFi();

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  setupLED();
  setupWiFi();
  setupServer();
}

void loop()
{
  MDNS.update();
  loopLED();
}

void setupWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  // MDNS init
  startMDNS();
  MDNS.addService("http", "tcp", 80);
}

void startMDNS()
{
  if (!MDNS.begin(hostName))
  {
    Serial.println("[ERROR] MDNS responder did not setup");
    while (1)
    {
      delay(1000);
    }
  }
  else
  {
    Serial.println("[INFO] MDNS setup is successful!");
  }
}