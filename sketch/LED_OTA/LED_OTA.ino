#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//OTAアップデート用のWiFiアクセスポイントの設定
const char *ssid = "ESP-UPDATE";
const char *password = "ESP32ESP32";

//LEDの設定
#define LED_R_PIN 25
#define LED_G_PIN 26
#define LED_B_PIN 27

#define LEDC_TIMER_BIT 8
#define LEDC_BASE_FREQ 12800
#define LED_R_CHANNEL 0
#define LED_G_CHANNEL 1
#define LED_B_CHANNEL 2

int cnt;

void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println("Booting");

    //LEDの設定
    Serial.println("init LED");
    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);
    ledcSetup(LED_R_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcSetup(LED_G_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcSetup(LED_B_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcAttachPin(LED_R_PIN, LED_R_CHANNEL);
    ledcAttachPin(LED_G_PIN, LED_G_CHANNEL);
    ledcAttachPin(LED_B_PIN, LED_B_CHANNEL);

    //WiFi,OTA 設定
    Serial.println("init WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.println("WiFi Connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }else{
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    Serial.println("init OTA");

    ArduinoOTA.setHostname("CubBleMonitor");
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
        })
        .onEnd([]() {
            Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR)
                Serial.println("End Failed");
        });

    Serial.println("Begin OTA");
    ArduinoOTA.begin();
    Serial.println("OTA Ready");

    cnt = 0;
}


void loop()
{
    ArduinoOTA.handle();

    if ((cnt / 256) % 3 == 0) {
        ledcWrite(LED_R_CHANNEL, cnt % 255);
        ledcWrite(LED_G_CHANNEL, 0);
        ledcWrite(LED_B_CHANNEL, 0);
    }

    if ((cnt / 256) % 3 == 1) {
        ledcWrite(LED_R_CHANNEL, 0);
        ledcWrite(LED_G_CHANNEL, cnt % 255);
        ledcWrite(LED_B_CHANNEL, 0);
    }
    if ((cnt / 256) % 3 == 2) {
        ledcWrite(LED_R_CHANNEL, 0);
        ledcWrite(LED_G_CHANNEL, 0);
        ledcWrite(LED_B_CHANNEL, cnt % 255);
    }
    cnt++;
    delay(10);
}
