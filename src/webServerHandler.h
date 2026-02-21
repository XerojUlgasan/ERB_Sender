#ifndef WEBSERVERHANDLER_H
#define WEBSERVERHANDLER_H

#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <time.h>
#include <sys/time.h>

#include "./class/senderProfile/senderProfile.h"
#include "./helpers/utils.h"

extern const String device_id;
extern void syncSystemTime();  // Forward declaration

AsyncWebServer server(80);
SenderProfile sender; // Global sender object

bool initializeWebServer(bool deviceIsSender, Preferences& pref) {

  Serial.println(sender.toJsonString());
 
  server.on( // TYPE
    "/type", 
    HTTP_GET, 
    [deviceIsSender, &pref](AsyncWebServerRequest *request) {
    String json = "{\"isSender\": " +(String)deviceIsSender + "}";

    request->send(
      200,
      "application/json",
      json);

      Serial.println(sender.toJsonString());
  });

  server.on(
    "/getProfile", // 204 if no profile, 200 if there's a profile
    HTTP_GET,
    [&pref](AsyncWebServerRequest *request){

        if(!sender.checkExist()){
            String json = "{\n\"profile_exist\": false\n}";

            request->send(204);
        }
        else{
            request->send(
                200,
                "application/json",
                sender.toJsonString()
            );
        }
    }
  );

  server.on(
    "/deleteProfile",
    HTTP_DELETE,
    [&pref](AsyncWebServerRequest *request){
      try
      {
        pref.begin("user_profile");
        pref.clear();
        Serial.println("USER PROFILE HAS BEEN DELETED!");
        pref.end();

        pref.begin("secret");
        pref.clear();
        Serial.println("SECRETS HAS BEEN DELETED!");
        pref.end();
      }
      catch(const std::exception& e)
      {
        Serial.println(e.what());
        request->send(500);  
      }
      

      request->send(200);
    }
  );

  server.on(
    "/setProfile", // SET PROFILE
    HTTP_POST,
    [&pref](AsyncWebServerRequest *request, JsonVariant &json){

        String firstname = json["firstname"].as<String>();
        String lastname = json["lastname"].as<String>();
        String middlename = json["middlename"].as<String>(); // Optional - can be null
        String birthdate = json["birthdate"].as<String>();
        String emergency_contact = json["emergency_contact"].as<String>();
        String emergency_person = json["emergency_person"].as<String>();
        String region = json["region"].as<String>();
        String city_municipality = json["city_municipality"].as<String>();
        String barangay = json["barangay"].as<String>();
        String contact = json["contact"].as<String>();

        // Validate required fields (middlename is optional)
        if(firstname.isEmpty() || lastname.isEmpty() || birthdate.isEmpty() || 
           emergency_contact.isEmpty() || emergency_person.isEmpty() || 
           region.isEmpty() || city_municipality.isEmpty() || 
           barangay.isEmpty() || contact.isEmpty()) {
            
            String errorJson = "{\"error\": \"Missing required fields. All fields are required except middlename.\"}";
            request->send(400, "application/json", errorJson);
            return;
        }

        sender.setSenderProfile(firstname, lastname, middlename, birthdate,
                               emergency_contact, emergency_person, region,
                               city_municipality, barangay, contact);

        request->send(200);

        Serial.println(sender.toJsonString());
    }
  );

  server.on(
    "/deleteProfile",
    HTTP_DELETE,
    [](AsyncWebServerRequest *request) {
      sender.deleteProfile();
      request->send(200);
    }
  );

  server.on(
    "/detectNetworks", // Returns all network 
    HTTP_GET,
    [](AsyncWebServerRequest *request){
        std::vector<String> ssid;
        std::vector<bool> isSecure;

        detectNetworks(ssid, isSecure);

        String ssidStr = vectorToJsonStringArr(ssid);
        String isSecureStr = vectorToJsonBoolArr(isSecure);

        Serial.println(ssidStr);
        Serial.println(isSecureStr);
        
        String json = "{";
        json += "\"networks\": " + ssidStr + ",";
        json += "\"isSecure\": " + isSecureStr;
        json += "}";

        Serial.println("Sending response...");
        request->send(200, "application/json", json);
        Serial.println("Response sent");
    });

    server.on(
      "/setNetwork",
      HTTP_POST,
      [](AsyncWebServerRequest *request, JsonVariant &json){
        String ssid = json["ssid"].as<String>();
        String pass = json["password"].as<String>();

        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(ssid, pass);

        int timeout = 10000; 
        int elapsed = 0;
        Serial.println("Connecting...");
        while (WiFi.status() != WL_CONNECTED && elapsed < timeout) {
          delay(100);
          elapsed += 100;
          Serial.print(".");
        }
        
        // Sync NTP time after WiFi connects (critical for SSL/TLS)
        if(WiFi.status() == WL_CONNECTED) {
          Serial.println("\nWiFi connected! Syncing system time...");
          syncSystemTime();
        }

        String jsonString;
        JsonDocument doc;
        doc["ssid"] = (WiFi.status() == WL_CONNECTED) ? WiFi.SSID() : "Not Connected";
        doc["isConnected"] = (WiFi.status() == WL_CONNECTED) ? true : false;
        doc["ip"] = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "192.168.4.1";

        serializeJson(doc, jsonString);
        
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("Connected!");
          request->send(200, "application/json", jsonString);
        } else {
          request->send(400, "application/json", jsonString);
        }
      }
    );

    server.on(
      "/wifiStatus",
      HTTP_GET,
      [](AsyncWebServerRequest *request){
        String jsonString;
        JsonDocument doc;
        doc["ssid"] = (WiFi.status() == WL_CONNECTED) ? WiFi.SSID() : "Not Connected";
        doc["isConnected"] = (WiFi.status() == WL_CONNECTED) ? true : false;
        doc["ip"] = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "192.168.4.1";

        serializeJson(doc, jsonString);

        request->send(
          200,
          "application/json",
          jsonString
        );
      }
    );

    server.on(
      "/confirmRegistration",
      HTTP_POST,
      [](AsyncWebServerRequest *request) {
        Serial.printf("Free heap before HTTPS: %d bytes\n", ESP.getFreeHeap());
        
        // Give WiFi connection time to stabilize
        delay(500);
        
        if(WiFi.status() != WL_CONNECTED){
          Serial.println("WiFi not connected during confirmRegistration!");
          request->send(503);  // Service Unavailable
          return;
        }
        
        if(sender.uploadToAPI(device_id)){
          request->send(200);
        }else{
          request->send(400);
        }
      }
    );

  server.begin();

  return true;
}

#endif
