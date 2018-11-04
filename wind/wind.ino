#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Defines
#define HALLPIN 2 // D4 (also LED_BUILDIN)

// Constants
const char *ssid = "";
const char *password = "";
const char* fingerprint = "19 6B 2D AA E4 3D 39 7D D7 70 E8 9B CE EE 2A 63 71 E0 0F E5";

// Global declarations
unsigned long startTime;
unsigned long readings[50];
int currentHallState = LOW;
int prevHallState = HIGH;
int rotations = 0;

void setup()
{
    Serial.begin(115200);

    //Start, configurare and connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // Verbonden.
    Serial.println("Connected!");

    // Access Point (SSID).
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // IP adres.
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // Setup pins
    pinMode(HALLPIN, INPUT);

    // Assing start variables
    startTime = millis();
}

void loop()
{
    currentHallState = digitalRead(HALLPIN);

    if (currentHallState == LOW && currentHallState != prevHallState)
    {
        rotations += 1;

        if (rotations % 2 == 0)
        {
            // Array do start at 0
            int index = (rotations / 2) - 1;
            int difference = millis() - startTime;

            // Use a offset of minus 2, since one complete rotation has two magnets
            readings[index] = difference;
        }
    }

    prevHallState = currentHallState;

    // Caculate the RPM every minute (300000ms = 5 minuten)
    if (millis() >= startTime + 10000)
    {
        sendWindData();
        startTime = millis();
        rotations = 0;
        memset(readings, 0, sizeof(readings));
    }
}

int calculateRpm()
{
    return (int)rotations / 2;
}

bool sendWindData()
{
    HTTPClient http;
    String payload;
    DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3));

    // Build JSON payload
    JsonObject &root = jsonBuffer.createObject();
    JsonArray &readingsArray = jsonBuffer.createArray();
    readingsArray.copyFrom(readings);

    root["readings"] = readingsArray;

    root.printTo(payload);

    // Build HTTP POST
    http.begin("https://weer.nielsvanderveer.nl/api/weather/wind", fingerprint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "Wemos D1 Mini - Windmeter");

    int httpCode = http.POST(payload);

    if (httpCode != 201)
    {
        Serial.println("Faild to send data.");
        Serial.println(httpCode);
        Serial.println(payload);
        return false;
    }

    http.end();

    return true;
}
