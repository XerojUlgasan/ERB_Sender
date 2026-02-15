#ifndef MYGPS_H
#define MYGPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>

#include "../GPSData.h"

class MyGps
{
private:
    TinyGPSPlus gps;
    HardwareSerial& GPS;
public:
    double lon, lat, alt, spd;
    uint32_t sat;
    bool isLocValid, isAltValid, isSpdValid;

    MyGps();
    ~MyGps();

    void begin();
    void getLocation();
    String locationToJsonString();
    JsonDocument locationToJson();
    GPSData getGPSDataStuct(String device_id, int& ping_count, bool isClick, bool isCancellation);
};

#endif