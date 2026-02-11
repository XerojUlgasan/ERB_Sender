#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>

#include "./class/senderProfile/senderProfile.h"
#include "./helpers/utils.h"

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
    "/setProfile", // SET PROFILE
    HTTP_POST,
    [&pref](AsyncWebServerRequest *request, JsonVariant &json){

        String fn, add, con_num, em_con_per, em_con_num;

        fn = json["fullname"].as<String>();
        add = json["address"].as<String>();
        con_num = json["contact_number"].as<String>();
        em_con_per = json["emergency_contact_person"].as<String>();
        em_con_num = json["emergency_contact_number"].as<String>();

        Serial.println(em_con_per);
        Serial.println(em_con_num);

        sender.setSenderProfile(fn, add, con_num, em_con_per, em_con_num);
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

  server.begin();

  return true;
}
