#ifndef WEBSERVERHANDLER_H
#define WEBSERVERHANDLER_H

#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <time.h>
#include <sys/time.h>

#include "./class/senderProfile/senderProfile.h"
#include "./class/EmergencyHistory.h"
#include "./helpers/utils.h"
#include "./helpers/wifiHelper.h"

extern const String device_id;

AsyncWebServer server(80);
SenderProfile sender; // Global sender object

const char* SAVED_NETWORKS_KEY = "saved_networks";
extern bool web_server_running;

bool initializeWebServer(bool deviceIsSender, Preferences& pref) {
  try
  {

    static bool routesRegistered = false;

    Serial.println(sender.toJsonString(device_id));
    
    if (!routesRegistered) {
      server.on( // TYPE
      "/type", 
      HTTP_GET, 
      [deviceIsSender, &pref](AsyncWebServerRequest *request) {
      String json = "{\"isSender\": " +(String)deviceIsSender + "}";

      request->send(
        200,
        "application/json",
        json);

        Serial.println(sender.toJsonString(device_id));
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
                  sender.toJsonString(device_id)
              );
          }
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

          sender.dummyProfile = {
              firstname,
              lastname,
              middlename,
              birthdate,
              emergency_contact,
              emergency_person,
              region,
              city_municipality,
              barangay,
              contact
          };

          // sender.setSenderProfile(firstname, lastname, middlename, birthdate,
          //                        emergency_contact, emergency_person, region,
          //                        city_municipality, barangay, contact);

          request->send(200);

          // Serial.println(sender.toJsonString(device_id));
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

      // server.on(
      //   "/saveNetwork",
      //   HTTP_POST,
      //   [&pref](AsyncWebServerRequest *request, JsonVariant &json) {
      //     String ssid = json["ssid"].as<String>();
      //     String pass = json["password"].as<String>();

      //     // Missing or invalid priority is treated as lowest priority.
      //     int requestedPriority = json["priority"].isNull() ? -1 : json["priority"].as<int>();

      //     if (ssid.isEmpty()) {
      //       request->send(400, "application/json", "{\"error\":\"ssid is required\"}");
      //       return;
      //     }

      //     pref.begin("secret");

      //     String storedNetworks = pref.getString(SAVED_NETWORKS_KEY, "[]");
      //     JsonDocument currentDoc;
      //     DeserializationError parseError = deserializeJson(currentDoc, storedNetworks);

      //     if (parseError || !currentDoc.is<JsonArray>()) {
      //       currentDoc.clear();
      //       currentDoc.to<JsonArray>();
      //     }

      //     JsonArray currentNetworks = currentDoc.as<JsonArray>();

      //     int insertIndex = currentNetworks.size();
      //     if (requestedPriority > 0) {
      //       insertIndex = requestedPriority - 1;
      //       if (insertIndex < 0) {
      //         insertIndex = 0;
      //       }
      //       if (insertIndex > (int)currentNetworks.size()) {
      //         insertIndex = currentNetworks.size();
      //       }
      //     }

      //     // Keep only one record per SSID by removing existing entry first.
      //     for (int i = 0; i < (int)currentNetworks.size(); i++) {
      //       if (currentNetworks[i]["ssid"].as<String>() == ssid) {
      //         currentNetworks.remove(i);
      //         if (i < insertIndex) {
      //           insertIndex--;
      //         }
      //         break;
      //       }
      //     }

      //     JsonDocument orderedDoc;
      //     JsonArray orderedNetworks = orderedDoc.to<JsonArray>();

      //     for (int i = 0; i <= (int)currentNetworks.size(); i++) {
      //       if (i == insertIndex) {
      //         JsonObject inserted = orderedNetworks.createNestedObject();
      //         inserted["ssid"] = ssid;
      //         inserted["password"] = pass;
      //       }

      //       if (i < (int)currentNetworks.size()) {
      //         JsonObject source = currentNetworks[i];
      //         JsonObject copied = orderedNetworks.createNestedObject();
      //         copied["ssid"] = source["ssid"].as<String>();
      //         copied["password"] = source["password"].as<String>();
      //       }
      //     }

      //     String output;
      //     serializeJson(orderedDoc, output);
      //     pref.putString(SAVED_NETWORKS_KEY, output);
      //     pref.end();

      //     JsonDocument responseDoc;
      //     responseDoc["saved"] = true;
      //     responseDoc["count"] = orderedNetworks.size();
      //     responseDoc["networks"] = orderedNetworks;

      //     String response;
      //     serializeJson(responseDoc, response);

      //     request->send(200, "application/json", response);
      //   }
      // );

      server.on(
        "/getSavedNetworks",
        HTTP_GET,
        [&pref](AsyncWebServerRequest *request) {
          pref.begin("secret", true);
          String storedNetworks = pref.getString(SAVED_NETWORKS_KEY, "[]");
          pref.end();

          JsonDocument storedDoc;
          DeserializationError parseError = deserializeJson(storedDoc, storedNetworks);

          if (parseError || !storedDoc.is<JsonArray>()) {
            storedDoc.clear();
            storedDoc.to<JsonArray>();
          }

          JsonArray networks = storedDoc.as<JsonArray>();

          JsonDocument responseDoc;
          responseDoc["count"] = networks.size();
          responseDoc["networks"] = networks;

          String response;
          serializeJson(responseDoc, response);

          request->send(200, "application/json", response);
        }
      );

      server.on(
        "/removeSavedNetwork",
        HTTP_DELETE,
        [&pref](AsyncWebServerRequest *request, JsonVariant &json) {
          String ssidToRemove = json["ssid"].as<String>();

          if (ssidToRemove.isEmpty()) {
            request->send(400, "application/json", "{\"error\":\"ssid is required\"}");
            return;
          }

          pref.begin("secret");
          String storedNetworks = pref.getString(SAVED_NETWORKS_KEY, "[]");
          
          JsonDocument currentDoc;
          DeserializationError parseError = deserializeJson(currentDoc, storedNetworks);

          if (parseError || !currentDoc.is<JsonArray>()) {
            pref.end();
            request->send(404, "application/json", "{\"error\":\"No saved networks found\"}");
            return;
          }

          JsonArray currentNetworks = currentDoc.as<JsonArray>();
          bool found = false;
          int removedIndex = -1;

          // Find and mark the network to remove
          for (int i = 0; i < (int)currentNetworks.size(); i++) {
            if (currentNetworks[i]["ssid"].as<String>() == ssidToRemove) {
              found = true;
              removedIndex = i;
              break;
            }
          }

          if (!found) {
            pref.end();
            JsonDocument errorDoc;
            errorDoc["error"] = "Network not found";
            errorDoc["ssid"] = ssidToRemove;
            
            String errorResponse;
            serializeJson(errorDoc, errorResponse);
            Serial.println(errorResponse);
            request->send(404, "application/json", errorResponse);
            return;
          }

          // Create new array without the removed network
          JsonDocument newDoc;
          JsonArray newNetworks = newDoc.to<JsonArray>();

          for (int i = 0; i < (int)currentNetworks.size(); i++) {
            if (i != removedIndex) {
              JsonObject source = currentNetworks[i];
              JsonObject copied = newNetworks.createNestedObject();
              copied["ssid"] = source["ssid"].as<String>();
              copied["password"] = source["password"].as<String>();
            }
          }

          // Save updated list
          String output;
          serializeJson(newDoc, output);
          pref.putString(SAVED_NETWORKS_KEY, output);
          pref.end();

          // Disconnect if currently connected to this network
          if (WiFi.status() == WL_CONNECTED && WiFi.SSID() == ssidToRemove) {
            Serial.printf("Disconnecting from %s as it was removed from saved networks\n", ssidToRemove.c_str());
            WiFi.disconnect(true);
          }

          // Send success response
          JsonDocument responseDoc;
          responseDoc["removed"] = true;
          responseDoc["ssid"] = ssidToRemove;
          responseDoc["remaining_count"] = newNetworks.size();

          String response;
          serializeJson(responseDoc, response);

          request->send(200, "application/json", response);
        }
      );

      server.on(
        "/setNetwork",
        HTTP_POST,
        [&pref](AsyncWebServerRequest *request, JsonVariant &json){
          String ssid = json["ssid"].as<String>();
          String pass = json["password"].as<String>();
          bool isSave = json["isSave"].as<bool>();

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
            if (!saveNetworkToPreferences(pref, ssid, pass, isSave)) {
              request->send(400, "application/json", "{\"error\":\"Invalid SSID for saving\"}");
              return;
            }

            forceStaDns(primaryDNS, secondaryDNS);
            Serial.println("Connected!");
            request->send(200, "application/json", jsonString);
          } else {
            Serial.println("Connection failed, cleaning up WiFi state...");
            WiFi.disconnect(true);  // Disconnect and clear credentials
            delay(100);             // Give time for cleanup
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

          //NOTE: THIS IS FOR AUTO DISABLE "AP" OF ESP32
          // if(WiFi.status() == WL_CONNECTED) {
          //   delay(1000);
          //   Serial.println("Wifi is already connected to " + WiFi.SSID() + ". Disconnecting...");
          //   WiFi.mode(WIFI_STA);
          // }
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

      server.on(
        "/getHistory",
        HTTP_GET,
        [&pref](AsyncWebServerRequest *request) {
          pref.begin("secret");
          
          // Get all emergency history records
          EmergencyRecord records[MAX_EMERGENCY_RECORDS];
          int count = getAllEmergencyHistory(pref, records, MAX_EMERGENCY_RECORDS);
          
          pref.end();
          
          // Build JSON response
          JsonDocument doc;
          JsonArray historyArray = doc.createNestedArray("history");
          
          for (int i = 0; i < count; i++) {
            JsonObject record = historyArray.createNestedObject();
            record["emergency_id"] = String(records[i].emergency_id);
            record["lon"] = records[i].lon;
            record["lat"] = records[i].lat;
            record["isStart"] = records[i].isStart;
          }
          
          doc["total"] = count;
          
          String jsonString;
          serializeJson(doc, jsonString);
          
          request->send(200, "application/json", jsonString);
        }
      );

      routesRegistered = true;
    }

    server.begin();
    
  }
  catch(const std::exception& e)
  {
    Serial.println(e.what());
    return false;
  }
  

  return true;
}

void stopWebServer() {
  if (!web_server_running) {
    return;
  }

  server.end();
  WiFi.mode(WIFI_STA);
  web_server_running = false;
  Serial.println("Web server stopped...");
  return;
}

void startWebserver(bool deviceIsSender, Preferences& pref) {
  WiFi.softAP(device_id, "Malopit123");
  if (web_server_running) {
    return;
  }

  web_server_running = initializeWebServer(deviceIsSender, pref);

  if (web_server_running) {
    Serial.println("Web server running...");
  } else {
    Serial.println("Web server failed to start.");
  }
  
  return;
}

#endif
