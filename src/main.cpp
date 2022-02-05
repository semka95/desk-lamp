#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
const char *hostName = "lamp";

#include "web.h"

#include <ESP_EEPROM.h>
#define INIT_ADDR 15
#define INIT_KEY 11
#define BR_ADDR 0
#define MODE_ADDR 1
#define RGB_ADDR 2
#define HSV_ADDR 5
#define COLTEMP_ADDR 9

#define CLK D7
#define DT D6
#define SW D5
#include <EncButton.h>
EncButton<EB_TICK, DT, CLK, SW> enc;
bool ignorEnc = false;

Led *led;
Led *WebServer::_led;

void startMDNS();
void setupWiFi();

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  EEPROM.begin(16);
  delay(50);

  if (EEPROM.read(INIT_ADDR) != INIT_KEY)
  {
    Serial.print("init eeprom\n");
    EEPROM.write(INIT_ADDR, INIT_KEY);
    EEPROM.put(BR_ADDR, Led::defBr);
    EEPROM.put(MODE_ADDR, Led::defMode);
    EEPROM.put(RGB_ADDR, Led::defRGB);
    EEPROM.put(HSV_ADDR, Led::defHSV);
    EEPROM.put(COLTEMP_ADDR, Led::defColTemp);
    EEPROM.commit();
  }
  byte bn;
  ledModes mode;
  Led::RGB rgb;
  Led::HSV hsv;
  Led::RGB colTemp;
  EEPROM.get(BR_ADDR, bn);
  EEPROM.get(MODE_ADDR, mode);
  EEPROM.get(RGB_ADDR, rgb);
  EEPROM.get(HSV_ADDR, hsv);
  EEPROM.get(COLTEMP_ADDR, colTemp);
  // Serial.printf("rgb %d %d %d\n", rgb.r, rgb.g, rgb.b);
  // Serial.printf("coltemp %d %d %d\n", colTemp.r, colTemp.g, colTemp.b);
  // Serial.printf("hsv %d %d %d\n", hsv.h, hsv.s, hsv.v);
  // Serial.printf("bn %d\n", bn);
  // Serial.printf("mode %d\n", mode);

  led = new Led(bn, mode, rgb, hsv, colTemp);
  setupWiFi();
  WebServer::_led = led;
  WebServer srv;
}

void loop()
{
  MDNS.update();

  enc.tick();

  if (enc.right() && !ignorEnc)
  {
    led->changeBrightness(BR_STEP);
  }
  if (enc.left() && !ignorEnc)
  {
    led->changeBrightness(-BR_STEP);
  }

  if (enc.press())
  {
    byte bn = led->getBrightness();
    ledModes mode = led->getCurMode();
    Led::RGB rgb = led->getRGB();
    Led::HSV hsv = led->getHSV();
    Led::RGB colTemp = led->getColTemp();

    if (bn == 0)
    {
      EEPROM.get(BR_ADDR, bn);
      led->setBrightness(bn);
      led->setMode(mode);
      ignorEnc = false;
    }
    else
    {

      EEPROM.put(BR_ADDR, bn);
      EEPROM.put(MODE_ADDR, mode);
      EEPROM.put(RGB_ADDR, rgb);
      EEPROM.put(HSV_ADDR, hsv);
      EEPROM.put(COLTEMP_ADDR, colTemp);
      EEPROM.commit();
      led->turnOff();
      ignorEnc = true;
    }
  }
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