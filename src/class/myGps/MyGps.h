#ifndef MYGPS_H
#define MYGPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>

#include "../GPSData.h"

// GPS Serial pins for ESP32 (adjust if using different pins)
#define GPS_RX_PIN 16  // ESP32 RX2 - connect to GPS TX
#define GPS_TX_PIN 17  // ESP32 TX2 - connect to GPS RX

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
    GPSData getGPSDataStuct(String device_id, int& ping_count, bool isClick, bool isCancellation, String emergency_id);
    void saveLocationToPreferences();
    void loadLocationFromPreferences();
};

#endif