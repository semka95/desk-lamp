#include "web.h"

AsyncWebServer server(80);

const String contentType = "text/html; charset=UTF-8";

WebServer::WebServer()
{
    if (!LittleFS.begin())
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    server.addHandler(new SPIFFSEditor(HTTP_USERNAME, HTTP_PASSWORD, LittleFS));
    server.on("/heap", HTTP_GET, heapHandler);
    server.on("/api/brightness/set", HTTP_GET, handleBrightness);
    server.on("/api/brightness/up", HTTP_GET, handleBrUp);
    server.on("/api/brightness/down", HTTP_GET, handleBrDown);
    server.on("/api/rgb", HTTP_GET, handleStaticRGB);
    server.on("/api/hsv", HTTP_GET, handleStaticHSV);
    server.on("/api/white", HTTP_GET, handleStaticWhite);
    server.on("/api/color_temp", HTTP_GET, handleColorTemp);

    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

    server.onNotFound(nfHandler);
    server.onFileUpload(fileUploadHandler);
    server.onRequestBody(requestBodyHandler);

    server.begin();
    Serial.printf("server started \n");
}

void WebServer::handleStaticRGB(AsyncWebServerRequest *request)
{
    if (!request->hasParam("r") || !request->hasParam("g") || !request->hasParam("b"))
    {
        request->send(400, contentType);
        return;
    }

    uint8_t r = request->getParam("r")->value().toInt();
    uint8_t g = request->getParam("g")->value().toInt();
    uint8_t b = request->getParam("b")->value().toInt();

    _led->setRGBMode(r, g, b);

    request->send(200, contentType);
    return;
}

void WebServer::handleStaticHSV(AsyncWebServerRequest *request)
{
    if (!request->hasParam("h") || !request->hasParam("s") || !request->hasParam("v"))
    {
        request->send(400, contentType);
        return;
    }

    uint16_t h = request->getParam("h")->value().toInt();
    uint8_t s = request->getParam("s")->value().toInt();
    uint8_t v = request->getParam("v")->value().toInt();

    _led->setHSVMode(h, s, v);

    request->send(200, contentType);
    return;
}

void WebServer::handleStaticWhite(AsyncWebServerRequest *request)
{
    _led->setWhiteMode();

    request->send(200, contentType);
    return;
}

void WebServer::handleBrightness(AsyncWebServerRequest *request)
{
    if (!request->hasParam("value"))
    {
        request->send(400, contentType);
        return;
    }

    uint8_t bn = request->getParam("value")->value().toInt();

    _led->setBrightness(bn);

    request->send(200, contentType);
    return;
}

void WebServer::handleColorTemp(AsyncWebServerRequest *request)
{
    if (!request->hasParam("value"))
    {
        request->send(400, contentType);
        return;
    }

    int kelvin = request->getParam("value")->value().toInt();

    Led::RGB rgb = _led->calcColTemp(kelvin);
    _led->setColorTemperature(rgb.r, rgb.g, rgb.b);

    request->send(200, contentType);
    return;
}

void WebServer::handleBrUp(AsyncWebServerRequest *request)
{
    uint8_t bn = 15;
    if (request->hasParam("value"))
    {
        bn = request->getParam("value")->value().toInt();
    }

    _led->changeBrightness(bn);
    request->send(200, contentType);
}

void WebServer::handleBrDown(AsyncWebServerRequest *request)
{
    int8_t bn = 15;
    if (request->hasParam("value"))
    {
        bn = request->getParam("value")->value().toInt();
    }

    _led->changeBrightness(-bn);
    request->send(200, contentType);
}

void WebServer::requestBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (!index)
        Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char *)data);
    if (index + len == total)
        Serial.printf("BodyEnd: %u\n", total);
}

void WebServer::fileUploadHandler(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
        Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char *)data);
    if (final)
        Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
}

void WebServer::heapHandler(AsyncWebServerRequest *request)
{
    request->send(200, contentType, String(ESP.getFreeHeap()));
}

void WebServer::nfHandler(AsyncWebServerRequest *request)
{
    Serial.printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
        Serial.printf("GET");
    else if (request->method() == HTTP_POST)
        Serial.printf("POST");
    else if (request->method() == HTTP_DELETE)
        Serial.printf("DELETE");
    else if (request->method() == HTTP_PUT)
        Serial.printf("PUT");
    else if (request->method() == HTTP_PATCH)
        Serial.printf("PATCH");
    else if (request->method() == HTTP_HEAD)
        Serial.printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
        Serial.printf("OPTIONS");
    else
        Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if (request->contentLength())
    {
        Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
        Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for (i = 0; i < headers; i++)
    {
        AsyncWebHeader *h = request->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for (i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isFile())
        {
            Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        }
        else if (p->isPost())
        {
            Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
        else
        {
            Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
    }

    request->send(404, contentType);
}