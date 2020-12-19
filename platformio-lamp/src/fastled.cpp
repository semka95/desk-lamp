#include <fastled.h>

#define NUM_LEDS 30
#define CLK D7
#define DT D6
#define SW D5
#include "FastLED.h"
#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);
#define PIN D2
CRGB leds[NUM_LEDS];
byte bn = 150;

void setupFastLED()
{
    FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(bn);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
    setColorTemperature(3500);

    enc1.setType(TYPE2);
}

void loopFastLED(HardwareSerial Serial)
{
    enc1.tick();

    if (enc1.isRight())
    {
        if (bn != 255)
        {
            bn += 15;
            FastLED.setBrightness(bn);
        }
    }
    if (enc1.isLeft())
    {
        if (bn != 0)
        {
            bn -= 15;
            FastLED.setBrightness(bn);
        }
    }

    FastLED.show();
}

void colorsRoutine(byte r, byte g, byte b)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(r, g, b);
    }
}

void setBrightness(byte br)
{
    bn = br;
    FastLED.setBrightness(bn);
}

void changeBrightness(int8_t br)
{
    if ((bn + br) < 0)
        bn = 0;
    else if ((bn + br) > 255)
        bn = 255;
    else
        bn += br;
    FastLED.setBrightness(bn);
}

// taken from Alexgyver/microLED
void setColorTemperature(int kelvin)
{
    float tmpKelvin, tmpCalc;
    uint8_t _r, _g, _b;

    kelvin = constrain(kelvin, 1000, 40000);
    tmpKelvin = kelvin / 100;

    // red
    if (tmpKelvin <= 66)
        _r = 255;
    else
    {
        tmpCalc = tmpKelvin - 60;
        tmpCalc = (float)pow(tmpCalc, -0.1332047592);
        tmpCalc *= (float)329.698727446;
        tmpCalc = constrain(tmpCalc, 0, 255);
        _r = tmpCalc;
    }

    // green
    if (tmpKelvin <= 66)
    {
        tmpCalc = tmpKelvin;
        tmpCalc = (float)99.4708025861 * log(tmpCalc) - 161.1195681661;
        tmpCalc = constrain(tmpCalc, 0, 255);
        _g = tmpCalc;
    }
    else
    {
        tmpCalc = tmpKelvin - 60;
        tmpCalc = (float)pow(tmpCalc, -0.0755148492);
        tmpCalc *= (float)288.1221695283;
        tmpCalc = constrain(tmpCalc, 0, 255);
        _g = tmpCalc;
    }

    // blue
    if (tmpKelvin >= 66)
        _b = 255;
    else if (tmpKelvin <= 19)
        _b = 0;
    else
    {
        tmpCalc = tmpKelvin - 10;
        tmpCalc = (float)138.5177312231 * log(tmpCalc) - 305.0447927307;
        tmpCalc = constrain(tmpCalc, 0, 255);
        _b = tmpCalc;
    }

    colorsRoutine(_r, _g, _b);
}