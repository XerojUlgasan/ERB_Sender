#include "./MyGps.h"

MyGps::MyGps() : GPS(Serial2)
{
    GPS.begin(9600, SERIAL_8N1, RX2, TX2);
}

MyGps::~MyGps()
{
}

void MyGps::getLocation() {
    Serial.println("Getting Location...");
    
    while (GPS.available()) {
        char c = GPS.read();
        gps.encode(c);
    }

    if(gps.location.isUpdated() && gps.location.isValid()){
        Serial.print("\nLatitude: ");
        Serial.println(gps.location.lat(), 6);

        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);

        Serial.print("Altitude: ");
        Serial.println(gps.altitude.meters());

        Serial.print("Satellites: ");
        Serial.println(gps.satellites.value());

        lat = gps.location.lat();
        lon = gps.location.lng();
        alt = gps.altitude.meters();
        sat = gps.satellites.value();

        if(gps.altitude.isValid()) {
            Serial.println("VALID ALTITUEDE ::::: " + (String)gps.altitude.meters());
        }
    }
}
