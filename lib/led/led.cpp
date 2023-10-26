#include <led.h>

const byte Led::defBr = 30;
const ledModes Led::defMode = ledModes::WHITE;
const Led::RGB Led::defRGB = Led::RGB{255, 0, 0};
const Led::HSV Led::defHSV = Led::HSV{13450, 100, 100};
const Led::RGB Led::defColTemp = Led::RGB{100, 100, 100};

void Led::setBrightness(byte br)
{
    if (br < _minBr)
    {
        br = _minBr;
    }
    if (br > _maxBr)
    {
        br = _maxBr;
    }
    _bn = br;
    _strip.setBrightness(_bn);
    _strip.show();
}

byte Led::getBrightness()
{
    return _bn;
}

void Led::changeBrightness(int8_t br)
{
    if ((_bn + br) < _minBr)
        _bn = _minBr;
    else if ((_bn + br) > _maxBr)
        _bn = _maxBr;
    else
        _bn += br;
    _strip.setBrightness(_bn);
    _strip.show();
}

void Led::turnOff()
{
    _bn = 0;
    _strip.setBrightness(_bn);
    _strip.show();
}

void Led::setWhiteMode()
{
    Serial.print("setWhiteMod\n");
    _minBr = 1;
    _maxBr = MAX_BR;
    _curMode = ledModes::WHITE;
    uint32_t color = _strip.Color(0, 0, 0, 255);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        _strip.setPixelColor(i, color);
    }
    _strip.show();
}

void Led::setRGBMode(byte r, byte g, byte b)
{
    Serial.print("setRGBMod\n");
    _minBr = 15;
    _maxBr = MAX_BR;
    _curMode = ledModes::RGB;
    setBrightness(_bn);
    _curRGB = RGB{r, g, b};
    uint32_t color = _strip.Color(r, g, b, 0);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        _strip.setPixelColor(i, color);
    }
    _strip.show();
}

Led::RGB Led::getRGB()
{
    return _curRGB;
}

void Led::setHSVMode(uint16_t h, byte s, byte v)
{
    Serial.print("setHSVMod\n");
    _minBr = 15;
    _maxBr = MAX_BR;
    _curMode = ledModes::HSV;
    setBrightness(_bn);
    _curHSV = HSV{h, s, v};
    uint32_t color = _strip.ColorHSV(h, s, v);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        _strip.setPixelColor(i, color);
    }
    _strip.show();
}

Led::HSV Led::getHSV()
{
    return _curHSV;
}

ledModes Led::getCurMode()
{
    return _curMode;
}

void Led::setMode(ledModes mode)
{
    switch (mode)
    {
    case ledModes::WHITE:
        setWhiteMode();
        break;
    case ledModes::RGB:
        setRGBMode(_curRGB.r, _curRGB.g, _curRGB.b);
        break;
    case ledModes::HSV:
        setHSVMode(_curHSV.h, _curHSV.s, _curHSV.v);
        break;
    case ledModes::COLTEMP:
        setColorTemperature(_curColTemp.r, _curColTemp.g, _curColTemp.b);
        break;
    }
}

// taken from Alexgyver/microLED
Led::RGB Led::calcColTemp(int kelvin)
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

    return Led::RGB{_r, _g, _b};
}

void Led::setColorTemperature(byte r, byte g, byte b)
{
    _minBr = 10;
    _maxBr = 200;
    _curMode = ledModes::COLTEMP;
    setBrightness(_bn);
    _curColTemp = RGB{r, g, b};
    uint32_t color = _strip.Color(r, g, b, 255);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        _strip.setPixelColor(i, color);
    }
    _strip.show();
}

Led::RGB Led::getColTemp()
{
    return _curColTemp;
}