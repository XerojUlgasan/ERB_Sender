#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <vector>
#include <ArduinoJson.h>

#include "./class/senderProfile/senderProfile.h"

AsyncWebServer server(80);
SenderProfile sender; // Global sender object

String appendPostBody(String &body, uint8_t *data, size_t len, size_t index, size_t total);
bool parseJsonToSenderProfile(String &jsonStr, String &fn, String &add, String &con_num, String &em_con_per, String &em_con_num);

String vectorToJsonStringArr(const std::vector<String>& vec);
String vectorToJsonBoolArr(const std::vector<bool>& vec);

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

        Serial.println("Connecting...");
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }

        Serial.println("Attempting");
        if (WiFi.status() == WL_CONNECTED) {
          Serial.print("WiFi is connected to: ");
          Serial.println(WiFi.SSID());
          Serial.println(WiFi.localIP());

          String response;
          DynamicJsonDocument doc(256);

          doc["ip"] = WiFi.localIP().toString();
          serializeJson(doc, response);
          Serial.println("RESPONSEEEEEEEEEEEEEEe : " + response);
          
          request->send(200, "application/json", response);

          Serial.println("Good");
        }else{
          request->send(400);
          Serial.println("Failed");
        }
      }
    );

  server.begin();

  return true;
}













bool parseJsonToSenderProfile(String &jsonStr, String &fn, String &add, String &con_num, String &em_con_per, String &em_con_num){
  JsonDocument doc;

  DeserializationError err = deserializeJson(doc, jsonStr);

  if(err){
    Serial.print("Parse failed: ");
    Serial.println(err.c_str());
    return false;
  }

  try
  {
    fn = doc["fullname"].as<String>();
    add = doc["address"].as<String>();
    con_num = doc["contact_number"].as<String>();
    em_con_per = doc["emergency_contact_person"].as<String>();
    em_con_num = doc["emergency_contact_number"].as<String>();
  }
  catch(const std::exception& e)
  {
    Serial.println(e.what());
    return false;
  }

  return true;
}

String appendPostBody(String &body, uint8_t *data, size_t len, size_t index, size_t total) {

    for (size_t i = 0; i < len; i++) {
        body += (char)data[i];
    }

    if (index + len == total) {
        return body;
    }
    return "";
}

String vectorToJsonStringArr(const std::vector<String>& vec) {
  String result;
  result.reserve(128);

  result += "[";

  for (size_t i = 0; i < vec.size(); i++) {
    result += "\"";
    result += vec[i];
    result += "\"";

    if (i < vec.size() - 1) result += ",";
  }

  result += "]";
  return result;
}

String vectorToJsonBoolArr(const std::vector<bool>& vec) {
  String result = "[";

  for (size_t i = 0; i < vec.size(); i++) {
    result += vec[i];
    if (i < vec.size() - 1) result += ", ";
  }

  result += "]";

  return result;
}