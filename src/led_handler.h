#include <Arduino.h>
#include <WiFi.h>
#include "./initializePins.h"
#include "./class/myGps/MyGps.h"

extern const bool deviceIsSender;
extern bool web_server_running;
extern bool lora_sending;
extern bool lora_receiving;
extern MyGps gps;
extern Preferences pref;

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
    return;
};

void webserverLed() {
    static int lastToggleState = -1;
    const int currentToggleState = digitalRead(toggle_web_server);

    if (currentToggleState == lastToggleState) {
        return;
    }

    if (currentToggleState == HIGH) {
        startWebserver(deviceIsSender, pref);
        WiFi.softAP(device_id, "Malopit123");
    } else {
        stopWebServer();
        WiFi.mode(WIFI_STA);
    }

    lastToggleState = currentToggleState;

    startWebserver(deviceIsSender, pref); // PANG TESTING NI ERNITS WAG AALISIN

    digitalWrite(webserver_led, (web_server_running) ? HIGH : LOW);
    return;
};

void gpsLed() {
    digitalWrite(gps_led, (gps.getGpsValidity()) ? HIGH : LOW);
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