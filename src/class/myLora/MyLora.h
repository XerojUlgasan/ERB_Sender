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

    MyLora(int nss, int rst, int dio);
    ~MyLora();

    void begin();
    void sendPacket(String message);
    void sendPacketStruct(GPSData &gpsData);
    bool parsePacketToStruct(const String &packet, GPSData &gpsData);
    bool consumeReceivedPacket(GPSData &gpsData);
    void startReceive();
    void stopReceive();
};

#endif
