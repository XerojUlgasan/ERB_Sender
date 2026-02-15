#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "initializePins.h"








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

#endif