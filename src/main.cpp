#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <esp_task_wdt.h>

#include "./helpers/wifiHelper.h"
#include "webServerHandler.h"

#include "./class/myGps/MyGps.h"


//TODO : Proper sanitation in Senderprofile.setProfile
//TODO : Save wifi networks
//TODO : History endpoint
//TODO : Encryption key generation
//TODO : Encryption key and user recording to cloud database

bool deviceIsSender = true;

Preferences pref;
MyGps mygps;

void setup() {
  esp_task_wdt_init(15, true);

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

  mygps.getLocation();
}
