#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Defines
#define HALLPIN 2 // D4 (also LED_BUILDIN)

// Constants
const char *ssid = "";
const char *password = "";

// Global declarations
unsigned long startTime;
int currentHallState = LOW;
int prevHallState = HIGH;
int rotations = 0;
int rpm = 0;

void setup()
{
    Serial.begin(115200);

    // Setup pins
    pinMode(HALLPIN, INPUT);

    // Assing start variables
    startTime = millis();
}

void loop()
{
    messuareRpm();
}

void messuareRpm()
{
    currentHallState = digitalRead(HALLPIN);

    if (currentHallState == LOW && currentHallState != prevHallState)
    {
        rotations += 1;
    }

    prevHallState = currentHallState;

    // Caculate the RPM every minute (60000 ms)
    if (millis() >= startTime + 10000)
    {
        rpm = calculateRpm();
        Serial.println(rpm);
        startTime = millis();
        rotations = 0;
    }
}

int calculateRpm()
{
    return (int)rotations / 2;
}

// bool sendAtmosphereData()
// {
//     HTTPClient http;
//     String payload;
//     DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3));

//     // Build JSON payload
//     JsonObject &root = jsonBuffer.createObject();
//     root["temperature"] = temperature;
//     root["humidity"] = humidity;
//     root["heatIndex"] = heatIndex;

//     root.printTo(payload);

//     // Build HTTP POST
//     http.begin("http://192.168.1.148:8080/api/weather/atmosphere");
//     http.addHeader("Content-Type", "application/json");
//     http.addHeader("User-Agent", "Wemos D1 Mini - Atmosphere");

//     int httpCode = http.POST(payload);

//     if (httpCode != 201)
//     {
//         drawCenterText("Faild to send data.");
//         Serial.println("Faild to send data.");
//         Serial.println(httpCode);
//         Serial.println(payload);
//         return false;
//     }

//     http.end();

//     return true;
// }
