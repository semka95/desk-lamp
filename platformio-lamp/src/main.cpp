#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

#include <secrets.h>
#include <led.h>

AsyncWebServer server(80);

const char *hostName = "lamp";
const String contentType = "text/html; charset=UTF-8";

void startMDNS();
void setupWiFi();
void setupServer();

void requestBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void fileUploadHandler(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);
void heapHandler(AsyncWebServerRequest *request);
void nfHandler(AsyncWebServerRequest *request);
void handleTest(AsyncWebServerRequest *request);
void handleBrUp(AsyncWebServerRequest *request);
void handleBrDown(AsyncWebServerRequest *request);

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  setupLED();

  setupWiFi();

  SPIFFS.begin();

  setupServer();
}

void loop()
{
  MDNS.update();
  loopLED();
}

void setupWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  // MDNS init
  startMDNS();
  MDNS.addService("http", "tcp", 80);
}

void startMDNS()
{
  if (!MDNS.begin(hostName))
  {
    Serial.println("[ERROR] MDNS responder did not setup");
    while (1)
    {
      delay(1000);
    }
  }
  else
  {
    Serial.println("[INFO] MDNS setup is successful!");
  }
}

void handleStaticRGB(AsyncWebServerRequest *request)
{
  if (!request->hasParam("r") || !request->hasParam("g") || !request->hasParam("b"))
  {
    request->send(400, contentType);
    return;
  }

  uint8_t r = request->getParam("r")->value().toInt();
  uint8_t g = request->getParam("g")->value().toInt();
  uint8_t b = request->getParam("b")->value().toInt();

  colorsRoutine(r, g, b);

  request->send(200, contentType);
  return;
}

void handleBrightness(AsyncWebServerRequest *request)
{
  if (!request->hasParam("value"))
  {
    request->send(400, contentType);
    return;
  }

  uint8_t bn = request->getParam("value")->value().toInt();

  setBrightness(bn);

  request->send(200, contentType);
  return;
}

void handleColorTemp(AsyncWebServerRequest *request)
{
  if (!request->hasParam("value"))
  {
    request->send(400, contentType);
    return;
  }

  int kelvin = request->getParam("value")->value().toInt();

  setColorTemperature(kelvin);

  request->send(200, contentType);
  return;
}

void setupServer()
{
  server.addHandler(new SPIFFSEditor(HTTP_USERNAME, HTTP_PASSWORD));
  server.on("/heap", HTTP_GET, heapHandler);
  server.on("/api/brightness/set", HTTP_GET, handleBrightness);
  server.on("/api/brightness/up", HTTP_GET, handleBrUp);
  server.on("/api/brightness/down", HTTP_GET, handleBrDown);
  server.on("/api/rgb", HTTP_GET, handleStaticRGB);
  server.on("/api/color_temp", HTTP_GET, handleColorTemp);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

  server.onNotFound(nfHandler);
  server.onFileUpload(fileUploadHandler);
  server.onRequestBody(requestBodyHandler);

  server.begin();
}

void handleBrUp(AsyncWebServerRequest *request)
{
  uint8_t bn = 15;
  if (request->hasParam("value"))
  {
    bn = request->getParam("value")->value().toInt();
  }

  changeBrightness(bn);
  request->send(200, contentType);
}

void handleBrDown(AsyncWebServerRequest *request)
{
  int8_t bn = 15;
  if (request->hasParam("value"))
  {
    bn = request->getParam("value")->value().toInt();
  }

  changeBrightness(-bn);
  request->send(200, contentType);
}

void requestBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  if (!index)
    Serial.printf("BodyStart: %u\n", total);
  Serial.printf("%s", (const char *)data);
  if (index + len == total)
    Serial.printf("BodyEnd: %u\n", total);
}

void fileUploadHandler(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
    Serial.printf("UploadStart: %s\n", filename.c_str());
  Serial.printf("%s", (const char *)data);
  if (final)
    Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
}

void heapHandler(AsyncWebServerRequest *request)
{
  request->send(200, contentType, String(ESP.getFreeHeap()));
}

void nfHandler(AsyncWebServerRequest *request)
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