#include <led.h>
#define NUM_LEDS 60
#define LED_PIN D4
#define MAX_BR 255
#define BR_STEP 15
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
byte bn = 150;

void setupLED()
{
    strip.begin();
    strip.show();
    strip.setBrightness(bn);
    strip.fill(strip.Color(255, 0, 0));
}

void loopLED()
{
    strip.show();
}

void RGBRoutine(byte r, byte g, byte b)
{
    uint32_t color = strip.Color(r, g, b, 0);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, color);
    }
}

void setBrightness(byte br)
{
    if (br == 0)
    {
        br = 1;
    }
    if (br > MAX_BR)
    {
        br = MAX_BR;
    }
    bn = br;
    strip.setBrightness(bn);
}

void changeBrightness(int8_t br)
{
    if ((bn + br) <= 0)
        bn = 1;
    else if ((bn + br) > MAX_BR)
        bn = MAX_BR;
    else
        bn += br;
    strip.setBrightness(bn);
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

    RGBRoutine(_r, _g, _b);
}