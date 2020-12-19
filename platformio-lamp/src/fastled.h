#include <Arduino.h>

void setupFastLED();
void loopFastLED(HardwareSerial Serial);
void colorsRoutine(byte r, byte g, byte b);
void setBrightness(byte bn);
void changeBrightness(int8_t br);
void setColorTemperature(int kelvin);