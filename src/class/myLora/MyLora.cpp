#include <Arduino.h>
#include <LoRa.h>

#include "MyLora.h"

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

    LoRa.setSpreadingFactor(8);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(6);
    LoRa.setTxPower(17);
    LoRa.enableCrc();
    LoRa.setSyncWord(0x1f);

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

void MyLora::startReceive(){
    LoRa.onReceive(onReceive);
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
    Serial.println("LoRa received a message...");   

    String msg = "";
    while (LoRa.available()) {
        msg += (char)LoRa.read();
    }

    Serial.println("Received: " + msg);

    return;
}
