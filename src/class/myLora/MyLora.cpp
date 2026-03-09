#include <Arduino.h>
#include <LoRa.h>
#include <Preferences.h>
#include <ArduinoJson.h>

#include "MyLora.h"

String MyLora::receivedMessage = "";
bool MyLora::packetReceived = false;

void onReceive(int packetSize);

MyLora::MyLora(int nss, int rst, int dio)
{
    _nss = nss;
    _rst = rst;
    _dio = dio;
}

MyLora::~MyLora(){}

void MyLora::begin() {
    LoRa.setPins(_nss, _rst, _dio);

    if(!LoRa.begin(433E6)){
        Serial.println("LoRa init failed!");
        ESP.restart();
    }

    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(62.5E3);
    LoRa.setCodingRate4(8);
    LoRa.setTxPower(20);
    LoRa.enableCrc();
    LoRa.setSyncWord(0x1f);
    LoRa.setPreambleLength(12);

    LoRa.onReceive(onReceive);

    Serial.println("LoRa has started...");
}

void MyLora::sendPacket(String message){

    Serial.println("Sending packet...");

    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();

    Serial.println("LoRa sent : " + message);

    return;
}

void MyLora::sendPacketStruct(GPSData &gpsData){
    Serial.println("Sending GPSData packet over LoRa...");

    JsonDocument doc;
    doc["lon"] = gpsData.lon;
    doc["lat"] = gpsData.lat;
    doc["alt"] = gpsData.alt;
    doc["spd"] = gpsData.spd;
    doc["device_id"] = gpsData.device_id;
    doc["emergency_id"] = gpsData.emergency_id;
    doc["ping_count"] = gpsData.ping_count;
    doc["is_click"] = gpsData.isClick;
    doc["is_cancel"] = gpsData.isCancellation;
    doc["is_loc_valid"] = gpsData.isLocValid;
    doc["is_alt_valid"] = gpsData.isAltValid;
    doc["is_spd_valid"] = gpsData.isSpdValid;

    String payload;
    serializeJson(doc, payload);

    LoRa.beginPacket();
    LoRa.print(payload);
    LoRa.endPacket();

    Serial.println("LoRa sent GPS JSON: " + payload);
}

bool MyLora::parsePacketToStruct(const String &packet, GPSData &gpsData) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, packet);
    if (err) {
        Serial.println("Failed to parse LoRa packet JSON");
        return false;
    }

    gpsData.lon = doc["lon"].as<float>();
    gpsData.lat = doc["lat"].as<float>();
    gpsData.alt = doc["alt"].as<float>();
    gpsData.spd = doc["spd"].as<float>();
    gpsData.device_id = doc["device_id"].as<String>();
    gpsData.emergency_id = doc["emergency_id"].as<String>();
    gpsData.ping_count = doc["ping_count"].as<int>();
    gpsData.isClick = doc["is_click"].as<bool>();
    gpsData.isCancellation = doc["is_cancel"].as<bool>();
    gpsData.isLocValid = doc["is_loc_valid"].as<bool>();
    gpsData.isAltValid = doc["is_alt_valid"].as<bool>();
    gpsData.isSpdValid = doc["is_spd_valid"].as<bool>();

    return true;
}

bool MyLora::consumeReceivedPacket(GPSData &gpsData) {
    if (!packetReceived) {
        return false;
    }

    String packet = receivedMessage;
    receivedMessage = "";
    packetReceived = false;

    if (packet.isEmpty()) {
        return false;
    }

    return parsePacketToStruct(packet, gpsData);
}

void MyLora::startReceive(){
    LoRa.receive();
    Serial.println("LoRa is listening...");

    return;
}

void MyLora::stopReceive(){
    LoRa.idle();
    Serial.println("LoRa is idling...");

    return;
}

void onReceive(int packetSize){
    if(packetSize == 0) return;
    
    MyLora::receivedMessage = "";

    while (LoRa.available()) {
        MyLora::receivedMessage += (char)LoRa.read();
    }

    MyLora::packetReceived = true;
    return;
}
