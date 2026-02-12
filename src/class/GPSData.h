#ifndef GPSDATA_H
#define GPSDATA_H

#pragma pack(push, 1)
struct GPSData
{
    float lon;
    float lat;
    float alt;
    float spd;
    int sat;
    bool isValid;
};
#pragma pack(pop)

#endif