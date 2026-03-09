#ifndef MYLORA_H
#define MYLORA_H

#include <Arduino.h>

#include "../GPSData.h"


class MyLora
{
private:
    int _nss, _rst, _dio;
public:
    static String receivedMessage;
    static bool packetReceived;
    static GPSData receivedGPSData;
    static bool structPacketReceived;

    MyLora(int nss, int rst, int dio);
    ~MyLora();

    void begin();
    void sendPacket(String message);
    void sendPacketStruct(GPSData &gpsData);
    void startReceive();
    void stopReceive();
    bool receivePacketStruct(GPSData &outData);
};

#endif
