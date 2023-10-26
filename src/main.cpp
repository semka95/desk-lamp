#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "defines.h"
ESPAsync_WiFiManager_Lite *ESPAsync_WiFiManager;

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
EncButton enc(DT, CLK, SW);
bool ignorEnc = false;

Led *led;
Led *WebServer::_led;

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
  ArduinoOTA.begin();
}

void loop()
{
  ArduinoOTA.handle();

  ESPAsync_WiFiManager->run();

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
  Serial.print(F("\nStarting ESPAsync_WiFi using "));
  Serial.print(FS_Name);
  Serial.print(F(" on "));
  Serial.println(ARDUINO_BOARD);
  Serial.println(ESP_ASYNC_WIFI_MANAGER_LITE_VERSION);

#if USING_MRD
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
#else
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
#endif

  ESPAsync_WiFiManager = new ESPAsync_WiFiManager_Lite();
  String AP_SSID = MY_AP_SSID;
  String AP_PWD = MY_AP_PASSWORD;

  // Set customized AP SSID and PWD
  ESPAsync_WiFiManager->setConfigPortal(AP_SSID, AP_PWD);

  // Optional to change default AP IP(192.168.4.1) and channel(10)
  // ESPAsync_WiFiManager->setConfigPortalIP(IPAddress(192, 168, 120, 1));
  ESPAsync_WiFiManager->setConfigPortalChannel(0);
  // ESPAsync_WiFiManager->setSTAStaticIPConfig(IPAddress(192, 168, 1, 69),
  //                                            IPAddress(192, 168, 1, 1),
  //                                            IPAddress(255, 255, 255, 0));

  ESPAsync_WiFiManager->begin();
}