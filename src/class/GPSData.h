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
    int ping_count;
    bool isClick;
    bool isCancellation;
    String access_key;
};
#pragma pack(pop)

#endif