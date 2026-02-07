#include <Arduino.h>
#include <ArduinoJson.h>

#include "./MyGps.h"


MyGps::MyGps() : GPS(Serial2)
{
    GPS.begin(9600, SERIAL_8N1, RX2, TX2);
}

MyGps::~MyGps()
{
}

void MyGps::getLocation() {
    Serial.println("Getting Location...");
    
    while (GPS.available()) {
        char c = GPS.read();
        gps.encode(c);
    }

    if(gps.location.isUpdated() && gps.location.isValid()){
        Serial.print("\nLatitude: ");
        Serial.println(gps.location.lat(), 6);

        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);

        Serial.print("Altitude: ");
        Serial.println(gps.altitude.meters());

        Serial.print("Satellites: ");
        Serial.println(gps.satellites.value());

        Serial.print("loc valid: ");
        Serial.println(gps.location.isValid());

        Serial.print("alt valid: ");
        Serial.println(gps.altitude.isValid());

        Serial.print("spd valid: ");
        Serial.println(gps.speed.isValid());

        lat = gps.location.lat();
        lon = gps.location.lng();
        alt = gps.altitude.meters();
        sat = gps.satellites.value();
        spd = gps.speed.kmph();
        isLocValid = gps.location.isValid();
        isAltValid = gps.altitude.isValid();
        isSpdValid = gps.speed.isValid();
    }

    return;
}

String MyGps::locationToJsonString(){
    this->getLocation();

    String jsonString;
    JsonDocument doc;

    doc["latitude"] = lat;
    doc["longitude"] = lon;
    doc["altitude"] = alt;
    doc["satellite"] = sat;
    doc["is_location_valid"] = isLocValid;
    doc["is_altitude_valid"] = isAltValid;
    doc["is_speed_valid"] = isSpdValid;

    serializeJson(doc, jsonString);

    return jsonString;
}

JsonDocument MyGps::locationToJson() {
    this->getLocation();

    JsonDocument doc;

    doc["latitude"] = lat;
    doc["longitude"] = lon;
    doc["altitude"] = alt;
    doc["satellite"] = sat;
    doc["is_location_valid"] = isLocValid;
    doc["is_altitude_valid"] = isAltValid;
    doc["is_speed_valid"] = isSpdValid;

    return doc;
}