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
};
#pragma pack(pop)

#endif