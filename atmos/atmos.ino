#include <DHT.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Defines
#define OLED_RESET 0 // D3
#define DHTPIN 13 // D7
#define DHTTYPE DHT22

// Inital setup
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(OLED_RESET);

// Constants
const char* ssid = "";
const char* password = "";
const char* fingerprint = "19 6B 2D AA E4 3D 39 7D D7 70 E8 9B CE EE 2A 63 71 E0 0F E5";
const int sleepTimeS = 600;

// Global declarations
float humidity;
float temperature;
float heatIndex;
int faildReads = 0;

void setup()
{
    Serial.begin(115200);

    // Disable WiFi at startup to save battery
    WiFi.mode( WIFI_OFF );
    WiFi.forceSleepBegin();
    delay(1);

    // Start DTH sensor
    dht.begin();

    // Start and configurate the OLED display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.display();
    display.clearDisplay();
}

void loop()
{
    bool atmosphereRead = readAtmospherData();

    if (!atmosphereRead)
    {
        drawCenterText("Faild to read DHT.");
        Serial.println("Faild to read DHT.");
    } else {
        sendAtmosphereData();
    }

    faildReads = 0;

    // Display the temperature data agains and go to sleep
    readAtmospherData();

    // Turn the ESP into sleep mode
    ESP.deepSleep(sleepTimeS * 1000000, WAKE_RF_DISABLED);
}

bool readAtmospherData()
{
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    // Check if any reads failed and exit early
    if (isnan(humidity) || isnan(temperature))
    {
        if (faildReads < 3) {
            delay(1000);
            faildReads = faildReads + 1;
            return readAtmospherData();
        }
        else
        {
            return false;
        }
    }

    heatIndex = dht.computeHeatIndex(temperature, humidity, false);

    // Display all data
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Temperature  " + String(temperature) + "C");
    display.println("Humidity     " + String(humidity) + "%");
    display.println("Heat index   " + String(heatIndex) + "C");
    display.display();

    faildReads = 0;

    return true;
}

bool sendAtmosphereData()
{
    connectToWiFi();

    HTTPClient http;
    String payload;
    DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3));

    // Build JSON payload
    JsonObject &root = jsonBuffer.createObject();
    root["temperature"] = temperature;
    root["humidity"] = humidity;
    root["heatIndex"] = heatIndex;

    root.printTo(payload);

    // Build HTTP POST
    http.begin("https://weer.nielsvanderveer.nl/api/weather/atmosphere", fingerprint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "Wemos D1 Mini - Atmosphere");

    int httpCode = http.POST(payload);

    if (httpCode != 201)
    {
        drawCenterText("Faild to send data.");
        Serial.println("Faild to send data.");
        Serial.println(httpCode);
        Serial.println(payload);
        return false;
    }

    http.end();

    return true;
}

void connectToWiFi() {
    //Start, configurare and connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        drawCenterText("Connecting...");
    }

    // Verbonden.
    drawCenterText("Connected!");
    Serial.println("Connected!");

    // Access Point (SSID).
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // IP adres.
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void drawCenterText(String text) {
    display.clearDisplay();
    display.setCursor(0, display.height() / 2);
    display.print(text);
    display.display();
}