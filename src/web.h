#include <Arduino.h>

#include <led.h>
#include <secrets.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <SPIFFSEditor.h>

void setupServer();

void handleStaticRGB(AsyncWebServerRequest *request);
void handleStaticWhite(AsyncWebServerRequest *request);
void handleBrightness(AsyncWebServerRequest *request);
void handleColorTemp(AsyncWebServerRequest *request);
void handleBrUp(AsyncWebServerRequest *request);
void handleBrDown(AsyncWebServerRequest *request);

void requestBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void fileUploadHandler(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);
void heapHandler(AsyncWebServerRequest *request);
void nfHandler(AsyncWebServerRequest *request);
