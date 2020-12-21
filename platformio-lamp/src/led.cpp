#include <led.h>

#define NUMLEDS 30
#define STRIP_PIN D2
#define COLOR_DEBTH 3
#include "microLED.h"

#define CLK D7
#define DT D6
#define SW D5
#include "GyverEncoder.h"

Encoder enc1(CLK, DT, SW);

microLED<NUMLEDS, STRIP_PIN, LED_WS2812, ORDER_GRB> strip;

void setupLED()
{
    strip.setBrightness(60);

    strip.clear();
    strip.show();
    delay(1);

    strip.fill(mYellow);
    strip.show();

    // enc1.setType(TYPE2);
}
