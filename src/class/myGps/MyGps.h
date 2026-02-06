#include <Arduino.h>
#include <TinyGPSPlus.h>

class MyGps
{
private:
    TinyGPSPlus gps;
    HardwareSerial& GPS;
public:
    double lon, lat, alt;
    uint32_t sat;

    MyGps();
    ~MyGps();

    void getLocation();
};