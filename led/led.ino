#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Defines
#define LED_PIN 5 // D1
#define LED_AMOUNT 60

// Inital setup
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_AMOUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Constants
const char *ssid = "";
const char *password = "";
const char *fingerprint = "19 6B 2D AA E4 3D 39 7D D7 70 E8 9B CE EE 2A 63 71 E0 0F E5";

void setup()
{
    Serial.begin(115200);

    // Strip settings
    strip.begin();
    strip.setBrightness(105);
    setStripColor(strip.Color(0, 0, 0));

    // Start, configurare and connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println(".");
    }

    Serial.println("Connected!");
}

void loop()
{
    float temperature = getTemperature();

    if (temperature >= 30)
    {
        setStripColor(strip.Color(255, 0, 25));
    }
    else if (temperature < 30 && temperature >= 20)
    {
        setStripColor(strip.Color(13, 198, 100));
    }
    else if (temperature < 20 && temperature >= 5)
    {
        setStripColor(strip.Color(13, 155, 198));
    }
    else
    {
        setStripColor(strip.Color(255, 255, 255));
    }

    delay(60000);
}

void setStripColor(uint32_t color)
{
    uint16_t i;

    for (i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, color);
    }

    delay(20);
    strip.show();
}

float getTemperature()
{
    HTTPClient http;
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(7) + 220;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    http.begin("http://hro-sma.nl/api/weatherupdates/device/18bc764b-16ba-4c06-a498-5490061b864a/latest/1");
    http.addHeader("X-Device-Id", "18bc764b-16ba-4c06-a498-5490061b864a");

    int httpCode = http.GET();
    if (httpCode == 200)
    {
        String json = http.getString();

        JsonArray &root = jsonBuffer.parseArray(json);
        JsonObject &root_0 = root[0];

        return root_0["temperatureC"];
    }
    else
    {
        Serial.println("Error when getting weahter data:" + httpCode);
    }
}