#include <Arduino.h>
#include <LoRa.h>
#include <Preferences.h>

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
    LoRa.setSignalBandwidth(41.7E3);
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
    Serial.println("Sending Struct....");

    Preferences pref;

    LoRa.beginPacket();
    LoRa.write((uint8_t*)&gpsData, sizeof(gpsData));
    LoRa.endPacket();

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

void onReceive(int packetSize){
    if(packetSize == 0) return;
    
    MyLora::receivedMessage = "";

    while (LoRa.available()) {
        MyLora::receivedMessage += (char)LoRa.read();
    }

    MyLora::packetReceived = true;
    return;
}
