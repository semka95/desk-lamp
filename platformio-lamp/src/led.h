#include <Arduino.h>

void setupLED();
void loopLED();
void RGBRoutine(byte r, byte g, byte b);
void whiteColourRoutine();
void setBrightness(byte bn);
void changeBrightness(int8_t br);
void setColorTemperature(int kelvin);