#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <esp_task_wdt.h>

#include "./helpers/wifiHelper.h"
#include "webServerHandler.h"


//TODO : Proper sanitation in Senderprofile.setProfile

bool deviceIsSender = true;

Preferences pref;

String vectorToString(const std::vector<String>& vec);


void setup() {
  esp_task_wdt_init(15, true); // 10 second timeout, panic on timeout

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ERBriwan", "Malopit123");
  Serial.begin(115200);

  if(initializeWebServer(deviceIsSender, pref)){
    Serial.println("Setup Done!");
  }else{
    Serial.println("Setup Failed!");
  }
  Serial.println("ASDWADASD");
}

void loop() {
  delay(2000);
  Serial.println("Loopping check");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi is connected to: ");
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
  }
}
