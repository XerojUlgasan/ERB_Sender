#ifndef GPSDATA_H
#define GPSDATA_H

#pragma pack(push, 1)
struct GPSData
{
    float lon;
    float lat;
    float alt;
    float spd;
    String device_id;
    String emergency_id;  // 5-char alphanumeric ID for emergency tracking
    int ping_count;
    bool isClick;
    bool isCancellation;
    bool isLocValid;
    bool isAltValid;
    bool isSpdValid;
    uint8_t bounce = 0;
};
#pragma pack(pop)

#endif