#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include "./class/senderProfile/senderProfile.h"

AsyncWebServer server(80);

String appendPostBody(String &body, uint8_t *data, size_t len, size_t index, size_t total);

bool initializeWebServer(bool deviceIsSender, Preferences& pref) {
  server.on(
    "/type", 
    HTTP_GET, 
    [deviceIsSender](AsyncWebServerRequest *request) {
    String json = "{\"isSender\": " +(String)deviceIsSender + "}";

    request->send(
      200,
      "application/json",
      json);

  });

  server.on(
    "/getProfile",
    HTTP_GET,
    [&pref](AsyncWebServerRequest *request){
        SenderProfile sender(pref);

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
    "/setProfile",
    HTTP_POST,
    [](AsyncWebServerRequest *request){
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    },
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        static String body = "";
        String completeBody = appendPostBody(body, data, len, index, total);

        Serial.println(completeBody);
    }
  );

  server.begin();

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