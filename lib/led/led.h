#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 60
#define MAX_BR 255
#define BR_STEP 15
#define LED_PIN D4

enum class ledModes : uint8_t
{
    WHITE,
    RGB,
    HSV,
    COLTEMP,
};

class Led
{
public:
    struct HSV
    {
        uint16_t h;
        byte s;
        byte v;
    };
    struct RGB
    {
        byte r;
        byte g;
        byte b;
    };

    static const byte defBr;
    static const ledModes defMode;
    static const RGB defRGB;
    static const HSV defHSV;
    static const RGB defColTemp;

    Led(byte bn = defBr,
        ledModes mode = defMode,
        RGB rgb = defRGB,
        HSV hsv = defHSV,
        RGB colTemp = defColTemp)
        : _bn(bn), _curMode(mode), _curRGB(rgb), _curHSV(hsv), _curColTemp(colTemp), _strip(Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800))
    {
        _strip.begin();
        _strip.setBrightness(_bn);
        setMode(_curMode);
        _strip.show();
    }

    void setBrightness(byte br);
    byte getBrightness();
    void changeBrightness(int8_t br);
    void turnOff();

    void setWhiteMode();

    void setRGBMode(byte r, byte g, byte b);
    RGB getRGB();

    void setHSVMode(uint16_t hue, byte sat, byte val);
    HSV getHSV();

    void setColorTemperature(byte r, byte g, byte b);
    RGB calcColTemp(int kelvin);
    RGB getColTemp();

    ledModes getCurMode();
    void setMode(ledModes mode);

private:
    byte _bn;
    ledModes _curMode;
    RGB _curRGB;
    HSV _curHSV;
    RGB _curColTemp;
    byte _minBr;
    byte _maxBr;
    Adafruit_NeoPixel _strip;
};
