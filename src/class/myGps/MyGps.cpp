#include <Arduino.h>
#include <ArduinoJson.h>

#include "./MyGps.h"


MyGps::MyGps() : GPS(Serial2)
{
    
}

MyGps::~MyGps()
{
}

void MyGps::begin() {
    GPS.begin(9600, SERIAL_8N1, RX2, TX2);
}

void MyGps::getLocation() {
    Serial.println("Getting Location...");
    
    while (GPS.available()) {
        char c = GPS.read();
        gps.encode(c);
    }

    if(gps.location.isUpdated()){
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
    // doc["is_location_valid"] = isLocValid;
    // doc["is_altitude_valid"] = isAltValid;
    // doc["is_speed_valid"] = isSpdValid;

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
    // doc["is_location_valid"] = isLocValid;
    // doc["is_altitude_valid"] = isAltValid;
    // doc["is_speed_valid"] = isSpdValid;

    return doc;
}

GPSData MyGps::getGPSDataStuct(String device_id, int& ping_count, bool isClick, bool isCancellation){
    ping_count++;

    GPSData data;

    data.lat = gps.location.lat();
    data.lon = gps.location.lng();
    data.alt = gps.altitude.meters();
    data.spd = gps.speed.kmph();
    data.device_id = device_id;
    data.ping_count = ping_count;
    data.isClick = isClick;
    data.isCancellation = isCancellation;

    Serial.println("LAT : " + (String)data.lat);
    Serial.println("LON : " + (String)data.lon);
    Serial.println("ALT : " + (String)data.alt);
    Serial.println("SPD : " + (String)data.spd);
    Serial.println("DID : " + (String)data.device_id);
    Serial.println("PNC : " + (String)data.ping_count);
    Serial.println("CLICK : " + (String)data.isClick);
    Serial.println("CANCEL : " + (String)data.isCancellation);

    return data;
};