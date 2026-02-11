#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>

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
};