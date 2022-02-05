#include <Arduino.h>

#include "led.h"
#include "secrets.h"

#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <SPIFFSEditor.h>

class WebServer
{
public:
    WebServer();

    // TODO: try to move methods to private or protected
    static void handleStaticRGB(AsyncWebServerRequest *request);
    static void handleStaticHSV(AsyncWebServerRequest *request);
    static void handleStaticWhite(AsyncWebServerRequest *request);
    static void handleBrightness(AsyncWebServerRequest *request);
    static void handleColorTemp(AsyncWebServerRequest *request);
    static void handleBrUp(AsyncWebServerRequest *request);
    static void handleBrDown(AsyncWebServerRequest *request);

    static void requestBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void fileUploadHandler(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);
    static void heapHandler(AsyncWebServerRequest *request);
    static void nfHandler(AsyncWebServerRequest *request);

    static Led *_led;
};