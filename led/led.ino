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
const char *fingerprint = "6728576b9bcfe220f1917c08f25413c51e371cb2";

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

    if (temperature > 25.00) {
        setStripColor(strip.Color(255, 0, 0));
    } else {
        setStripColor(strip.Color(0, 255, 0));
    }

    delay(5000);
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
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(6) + 140;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    http.begin("https://weer.nielsvanderveer.nl/api/weather/atmosphere/latest", fingerprint);

    int httpCode = http.GET();
    if (httpCode == 200)
    {
        String json = http.getString();
        Serial.println(json);

        JsonArray &root = jsonBuffer.parseArray(json);
        JsonObject &root_0 = root[0];

        return root_0["heatindex"];
    }
    else
    {
        Serial.println("Error when getting weahter data:" + httpCode);
    }
}