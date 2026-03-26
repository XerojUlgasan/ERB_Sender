#include <Arduino.h>
#include <WiFi.h>
#include "./initializePins.h"
#include "./class/myGps/MyGps.h"


extern bool web_server_running;
extern bool lora_sending;
extern bool lora_receiving;
extern MyGps gps;

void wifiLed();
void webserverLed();
void gpsLed();
void loraLed();

void startLeds() {
    // should run on async/freeRTOS
    wifiLed();
    webserverLed();
    gpsLed();
    loraLed();
};

void wifiLed() {
    digitalWrite(wifi_led, (WiFi.status() == WL_CONNECTED) ? HIGH : LOW);
    Serial.println("WIFI IS ON!!!!");
    return;
};

void webserverLed() {
    digitalWrite(webserver_led, (web_server_running) ? HIGH : LOW);
    Serial.println("WEBSERVER IS ON!!!!");
    return;
};

void gpsLed() {
    digitalWrite(gps_led, (gps.getGpsValidity()) ? HIGH : LOW);
    Serial.println("GPS IS ON!!!!");
    return;
}

void loraLed() {
    // Independent non-blocking timing states for send/receive patterns
    static unsigned long lastSendToggle = 0;
    static unsigned long lastReceiveToggle = 0;
    static bool sendLedState = false;
    static bool receiveLedState = false;

    const unsigned long now = millis();

    if (lora_receiving) {
        // Fast blink while receiving: 120ms ON / 120ms OFF
        const unsigned long interval = 120;
        if (now - lastReceiveToggle >= interval) {
            lastReceiveToggle = now;
            receiveLedState = !receiveLedState;
            digitalWrite(lora_r_led, receiveLedState ? HIGH : LOW);
            Serial.println("LORA IS RECEIVING!!!!");
        }
    } else {
        receiveLedState = false;
        digitalWrite(lora_r_led, LOW);
    }

    if (lora_sending) {
        // Slow blink while sending: 500ms ON / 500ms OFF
        const unsigned long interval = 500;
        if (now - lastSendToggle >= interval) {
            lastSendToggle = now;
            sendLedState = !sendLedState;
            digitalWrite(lora_s_led, sendLedState ? HIGH : LOW);
            Serial.println("LORA IS SENDING!!!!");

        }
    } else {
        sendLedState = false;
        digitalWrite(lora_s_led, LOW);
    }

    return;
}