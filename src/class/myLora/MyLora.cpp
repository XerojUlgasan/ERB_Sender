#include <Arduino.h>
#include <LoRa.h>
#include <Preferences.h>

#include "MyLora.h"

extern bool lora_sending;
extern bool lora_receiving;

String MyLora::receivedMessage = "";
bool MyLora::packetReceived = false;
GPSData MyLora::receivedGPSData = {};
bool MyLora::structPacketReceived = false;

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
    lora_sending = true;

    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    LoRa.receive();

    lora_sending = false;

    Serial.println("LoRa sent : " + message);

    return;
}

void MyLora::sendPacketStruct(GPSData &gpsData){
    Serial.println("Sending Struct....");
    lora_sending = true;

    Preferences pref;

    Serial.println(gpsData.device_id);
    Serial.println(gpsData.emergency_id);
    Serial.println(gpsData.bounce);

    LoRa.beginPacket();
    LoRa.write((uint8_t*)&gpsData, sizeof(gpsData));
    LoRa.endPacket();
    LoRa.receive();

    lora_sending = false;

    Serial.println("Lora Send Struct");
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

bool MyLora::receivePacketStruct(GPSData &outData){
    if(!structPacketReceived) return false;
    
    outData = receivedGPSData;
    structPacketReceived = false;
    lora_receiving = false;
    return true;
}

void onReceive(int packetSize){
    Serial.println("PACKET RECEIVED!");
    if(packetSize == 0) return;
    
    // Check if packet size matches GPSData struct
    if(packetSize == sizeof(GPSData)) {
        // Read binary struct
        uint8_t buffer[sizeof(GPSData)];
        int bytesRead = 0;
        
        while(LoRa.available() && bytesRead < packetSize) {
            buffer[bytesRead++] = LoRa.read();
        }
        
        if(bytesRead == sizeof(GPSData)) {
            memcpy(&MyLora::receivedGPSData, buffer, sizeof(GPSData));
            MyLora::structPacketReceived = true;
            lora_receiving = true;
        }
    } else {
        // Fall back to string message
        MyLora::receivedMessage = "";
        while (LoRa.available()) {
            MyLora::receivedMessage += (char)LoRa.read();
        }
        MyLora::packetReceived = true;
    }
    return;
}
