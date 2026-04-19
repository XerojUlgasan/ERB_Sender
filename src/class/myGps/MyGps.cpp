#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#include "./MyGps.h"


MyGps::MyGps() : GPS(Serial2)
{
    
}

MyGps::~MyGps()
{
}

void MyGps::begin() {
    Serial.println("Initializing GPS...");
    GPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.print("GPS Serial started on RX:");
    Serial.print(GPS_RX_PIN);
    Serial.print(" TX:");
    Serial.println(GPS_TX_PIN);
    loadLocationFromPreferences();
}

void MyGps::getLocation() {
    Serial.println("Getting Location...");
    
    int bytesAvailable = 0;
    while (GPS.available()) {
        char c = GPS.read();
        Serial.print(c);  // Debug: Print raw NMEA sentences (GPGGA, GPRMC, etc.)
        gps.encode(c);
        bytesAvailable++;
    }
    
    if (bytesAvailable == 0) {
        Serial.println("No GPS data available!");
    } else {
        Serial.print("\nRead ");
        Serial.print(bytesAvailable);
        Serial.println(" bytes from GPS");

        Serial.println(gps.time.hour());
        Serial.println(gps.time.minute());
        Serial.println(gps.time.second());
        Serial.println(gps.satellites.value());
    }

    // if(gps.location.isUpdated()){
        Serial.print("\nLatitude: ");
        Serial.println(gps.location.lat(), 6);

        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);

        Serial.print("Altitude: ");
        Serial.println(gps.altitude.meters());

        Serial.print("Satellites: ");
        Serial.println(gps.satellites.value());

        Serial.print("loc valid: ");
        Serial.println(gps.location.isValid());

        Serial.print("alt valid: ");
        Serial.println(gps.altitude.isValid());

        Serial.print("spd valid: ");
        Serial.println(gps.speed.isValid());

        lat = gps.location.lat();
        lon = gps.location.lng();
        alt = gps.altitude.meters();
        sat = gps.satellites.value();
        spd = gps.speed.kmph();
        isLocValid = gps.location.isValid();
        isAltValid = gps.altitude.isValid();
        isSpdValid = gps.speed.isValid();
        
        // Save valid location to Preferences for recovery after power loss
        if (isLocValid) {
            saveLocationToPreferences();
        }
    // }

    return;
}

String MyGps::locationToJsonString(){
    this->getLocation();

    String jsonString;
    JsonDocument doc;

    doc["latitude"] = lat;
    doc["longitude"] = lon;
    doc["altitude"] = alt;
    doc["satellite"] = sat;
    // doc["is_location_valid"] = isLocValid;
    // doc["is_altitude_valid"] = isAltValid;
    // doc["is_speed_valid"] = isSpdValid;

    serializeJson(doc, jsonString);

    return jsonString;
}

JsonDocument MyGps::locationToJson() {
    this->getLocation();

    JsonDocument doc;

    doc["latitude"] = lat;
    doc["longitude"] = lon;
    doc["altitude"] = alt;
    doc["satellite"] = sat;
    // doc["is_location_valid"] = isLocValid;
    // doc["is_altitude_valid"] = isAltValid;
    // doc["is_speed_valid"] = isSpdValid;

    return doc;
}

GPSData MyGps::getGPSDataStuct(String device_id, int& ping_count, bool isClick, bool isCancellation, String emergency_id){
    ping_count++;
    
    // Ensure we have the latest GPS data before creating packet
    getLocation();

    GPSData data;
    Preferences pref;
    pref.begin("secret");

    // Get current GPS values
    double current_lat = gps.location.lat();
    double current_lon = gps.location.lng();
    double current_alt = gps.altitude.meters();
    double current_spd = gps.speed.kmph();
    
    // Check if GPS data is not available or invalid (0 or no fix)
    if (!gps.location.isValid() || current_lat == 0 || current_lon == 0) {
        Serial.println("GPS data unavailable, loading from Preferences...");
        loadLocationFromPreferences();
        data.lat = lat;
        data.lon = lon;
        data.alt = alt;
        data.spd = spd;
    } else {
        // Use current valid GPS data
        data.lat = current_lat;
        data.lon = current_lon;
        data.alt = current_alt;
        data.spd = current_spd;
    }
    
    data.device_id = device_id;
    data.emergency_id = emergency_id;
    data.ping_count = ping_count;
    data.isClick = isClick;
    data.isCancellation = isCancellation;
    data.isLocValid = gps.location.isValid();
    data.isAltValid = gps.altitude.isValid();
    data.isSpdValid = gps.speed.isValid();

    pref.end();
    Serial.println("LAT : " + (String)data.lat);
    Serial.println("LON : " + (String)data.lon);
    Serial.println("ALT : " + (String)data.alt);
    Serial.println("SPD : " + (String)data.spd);
    Serial.println("DID : " + (String)data.device_id);
    Serial.println("PNC : " + (String)data.ping_count);
    Serial.println("CLICK : " + (String)data.isClick);
    Serial.println("CANCEL : " + (String)data.isCancellation);
    Serial.println("EMERGENCY ID : " + (String)data.emergency_id);
    Serial.println("LOC_VALID : " + (String)data.isLocValid);
    Serial.println("ALT_VALID : " + (String)data.isAltValid);
    Serial.println("SPD_VALID : " + (String)data.isSpdValid);
    Serial.println("SATELLITES : " + (String)gps.satellites.value());
    return data;
}

void MyGps::saveLocationToPreferences() {
    Preferences pref;
    pref.begin("gps", false);
    
    pref.putDouble("last_lat", lat);
    pref.putDouble("last_lon", lon);
    pref.putDouble("last_alt", alt);
    pref.putDouble("last_spd", spd);
    
    pref.end();
    
    Serial.println("Location saved to Preferences");
}

void MyGps::loadLocationFromPreferences() {
    Preferences pref;
    pref.begin("gps", true);  // read-only mode
    
    lat = pref.getDouble("last_lat", 0.0);
    lon = pref.getDouble("last_lon", 0.0);
    alt = pref.getDouble("last_alt", 0.0);
    spd = pref.getDouble("last_spd", 0.0);
    
    pref.end();
    
    Serial.println("Location loaded from Preferences");
    Serial.println("Previous LAT: " + (String)lat);
    Serial.println("Previous LON: " + (String)lon);
}

bool MyGps::getGpsValidity() {
    while (GPS.available()) {
        char c = GPS.read();
        gps.encode(c);
    }
    return gps.location.isValid();
}