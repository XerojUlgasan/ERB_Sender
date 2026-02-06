#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <esp_task_wdt.h>

#include "./helpers/wifiHelper.h"
#include "webServerHandler.h"

#include "./class/myGps/MyGps.h"
#include "class/myLora/MyLora.h"


//TODO : Proper sanitation in Senderprofile.setProfile
//TODO : Save wifi networks
//TODO : History endpoint
//TODO : Encryption key generation
//TODO : Encryption key and user recording to cloud database

const bool deviceIsSender = true;

Preferences pref;
MyGps mygps;
MyLora lora(5, 14, 26);

void setup() {
  esp_task_wdt_init(15, true);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ERBriwan", "Malopit123");
  Serial.begin(115200);

  if(initializeWebServer(deviceIsSender, pref)){
    Serial.println("Setup Done!");
  }else{
    Serial.println("Setup Failed!");
    ESP.restart();
  } 
  Serial.println("ASDWADASD");
  lora.startReceive();
}

void loop() {

  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.print("WiFi is connected to: ");
  //   Serial.println(WiFi.SSID());
  //   Serial.println(WiFi.localIP());
  // }

  mygps.getLocation();
  lora.sendPacket("From Sender..............");
  delay(5000);
}
