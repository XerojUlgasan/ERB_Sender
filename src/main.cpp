#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include "webServerHandler.h"

bool deviceIsSender = true;

Preferences pref;

bool initializeWebServer(bool deviceIsSender, Preferences& pref);

void setup() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin("Ulgasan", "XerojHaha123?");
  WiFi.softAP("ERBriwan", "Malopit123");
  Serial.begin(115200);

  initializeWebServer(deviceIsSender, pref);

  Serial.println("Setup Done!");
}

void loop() {
  delay(2000);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi is connected to: ");
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
  }

}

