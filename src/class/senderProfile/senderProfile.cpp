#include "senderProfile.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

Profile SenderProfile::dummyProfile = {};
String SenderProfile::device_id = "";
String SenderProfile::api_url = "https://er-briwan-api.vercel.app/device/registerUser";
String SenderProfile::ping_url = "https://er-briwan-api.vercel.app/ping";
String SenderProfile::event_url = "https://er-briwan-api.vercel.app/device/recordPing";

SenderProfile::SenderProfile()
{
    applyPreferences();
}

SenderProfile::~SenderProfile()
{
    Serial.println("Sender Profile with " + myProfile.firstname + " " + myProfile.lastname + " is destroyed!");
}

bool SenderProfile::checkExist() {
    applyPreferences();

    // All fields are required except middlename
    if(myProfile.firstname.isEmpty() || myProfile.lastname.isEmpty() || 
       myProfile.birthdate.isEmpty() || myProfile.emergency_contact.isEmpty() || 
       myProfile.emergency_person.isEmpty() || myProfile.region.isEmpty() || 
       myProfile.city_municipality.isEmpty() || myProfile.barangay.isEmpty() || 
       myProfile.contact.isEmpty()){
        return false;
    }else{
        return true;
    }
}

String SenderProfile::toJsonString(const String &deviceId) {

    applyPreferences();

    this->senderPref.begin("secret");
    bool isUserRegistered = senderPref.getBool("hasUser");
    String user_id = senderPref.getString("user_id");
    
    String json = "{";
    json += "\"firstname\":\"" + myProfile.firstname + "\",";
    json += "\"lastname\":\"" + myProfile.lastname + "\",";
    json += "\"middlename\":\"" + myProfile.middlename + "\",";
    json += "\"birthdate\":\"" + myProfile.birthdate + "\",";
    json += "\"emergency_contact\":\"" + myProfile.emergency_contact + "\",";
    json += "\"emergency_person\":\"" + myProfile.emergency_person + "\",";
    json += "\"region\":\"" + myProfile.region + "\",";
    json += "\"city_municipality\":\"" + myProfile.city_municipality + "\",";
    json += "\"barangay\":\"" + myProfile.barangay + "\",";
    json += "\"contact\":\"" + myProfile.contact + "\",";
    json += "\"user_id\":\"" + user_id + "\",";
    json += "\"device_id\":\"" + deviceId + "\",";
    json += "\"isUserRegistered\":" + String(isUserRegistered ? "true" : "false");
    json += "}";

    this->senderPref.end();
    
    return json;
}

void SenderProfile::setSenderProfile(String firstname, String lastname, String middlename, String birthdate,
                                    String emergency_contact, String emergency_person, String region,
                                    String city_municipality, String barangay, String contact) {
    myProfile.firstname = firstname;
    myProfile.lastname = lastname;
    myProfile.middlename = middlename;
    myProfile.birthdate = birthdate;
    myProfile.emergency_contact = emergency_contact;
    myProfile.emergency_person = emergency_person;
    myProfile.region = region;
    myProfile.city_municipality = city_municipality;
    myProfile.barangay = barangay;
    myProfile.contact = contact;

    // Save to persistent storage immediately
    senderPref.begin("user_profile");
    senderPref.putString("firstname", myProfile.firstname);
    senderPref.putString("lastname", myProfile.lastname);
    senderPref.putString("middlename", myProfile.middlename);
    senderPref.putString("birthdate", myProfile.birthdate);
    senderPref.putString("em_contact", myProfile.emergency_contact);
    senderPref.putString("em_person", myProfile.emergency_person);
    senderPref.putString("region", myProfile.region);
    senderPref.putString("city_muni", myProfile.city_municipality);
    senderPref.putString("barangay", myProfile.barangay);
    senderPref.putString("contact", myProfile.contact);
    senderPref.end();

    Serial.println("Profile saved to persistent storage");

    return;
}

Profile SenderProfile::getProfile() {
    return myProfile;
}

void SenderProfile::deleteProfile() {
    this->senderPref.begin("user_profile");
    this->senderPref.clear();
    Serial.println("PROFIEL DELETED");
    this->senderPref.end();

    this->senderPref.begin("secret");
    this->senderPref.clear();
    Serial.println("SECRET DELETED");
    this->senderPref.end();
}

void SenderProfile::applyPreferences(){
    senderPref.begin("user_profile");

    myProfile.firstname = senderPref.getString("firstname", "");
    Serial.print("Firstname: ");
    Serial.println(myProfile.firstname);

    myProfile.lastname = senderPref.getString("lastname", "");
    Serial.print("Lastname: ");
    Serial.println(myProfile.lastname);

    myProfile.middlename = senderPref.getString("middlename", "");
    Serial.print("Middlename: ");
    Serial.println(myProfile.middlename);

    myProfile.birthdate = senderPref.getString("birthdate", "");
    Serial.print("Birthdate: ");
    Serial.println(myProfile.birthdate);

    myProfile.emergency_contact = senderPref.getString("em_contact", "");
    Serial.print("Emergency Contact: ");
    Serial.println(myProfile.emergency_contact);

    myProfile.emergency_person = senderPref.getString("em_person", "");
    Serial.print("Emergency Person: ");
    Serial.println(myProfile.emergency_person);

    myProfile.region = senderPref.getString("region", "");
    Serial.print("Region: ");
    Serial.println(myProfile.region);

    myProfile.city_municipality = senderPref.getString("city_muni", "");
    Serial.print("City/Municipality: ");
    Serial.println(myProfile.city_municipality);

    myProfile.barangay = senderPref.getString("barangay", "");
    Serial.print("Barangay: ");
    Serial.println(myProfile.barangay);

    myProfile.contact = senderPref.getString("contact", "");
    Serial.print("Contact: ");
    Serial.println(myProfile.contact);
    
    senderPref.end();

    return;
}

void SenderProfile::checkPreferences(){
    senderPref.begin("user_profile");

    Serial.println("Firstname: " + senderPref.getString("firstname", ""));
    Serial.println("Lastname: " + senderPref.getString("lastname", ""));
    Serial.println("Middlename: " + senderPref.getString("middlename", ""));
    Serial.println("Birthdate: " + senderPref.getString("birthdate", ""));
    Serial.println("Emergency Contact: " + senderPref.getString("em_contact", ""));
    Serial.println("Emergency Person: " + senderPref.getString("em_person", ""));
    Serial.println("Region: " + senderPref.getString("region", ""));
    Serial.println("City/Municipality: " + senderPref.getString("city_muni", ""));
    Serial.println("Barangay: " + senderPref.getString("barangay", ""));
    Serial.println("Contact: " + senderPref.getString("contact", ""));
    senderPref.end();
    return;
}

bool SenderProfile::uploadToAPI(String deviceId) {
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi not connected!");
        return false;
    }
    
    // if(!SenderProfile::checkExist()){
    //     Serial.println("Error in user profile maybe lack of field or does not exists.");
    //     return false;
    // }

    JsonDocument doc;
    doc["user_fn"] = dummyProfile.firstname;
    doc["user_ln"] = dummyProfile.lastname;
    doc["birthdate"] = dummyProfile.birthdate;
    doc["region"] = dummyProfile.region;
    doc["city_minucipality"] = dummyProfile.city_municipality;
    doc["barangay"] = dummyProfile.barangay;
    doc["contact"] = dummyProfile.contact;
    doc["em_contact"] = dummyProfile.emergency_contact;
    doc["em_person"] = dummyProfile.emergency_person;
    doc["device_id"] = deviceId;

    if(dummyProfile.middlename.isEmpty()){
        doc["user_mn"] = nullptr;
    }else{
        doc["user_mn"] = dummyProfile.middlename;
    }

    String payload;
    serializeJson(doc, payload);
    Serial.printf("Payload size: %d bytes\n", payload.length());

    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected before POST attempt!");
        return false;
    }
    
    // Force garbage collection before SSL attempt
    esp_task_wdt_reset();
    delay(200);
    
    // Create fresh client for each attempt
    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(30);
    
    HTTPClient http;
    http.setTimeout(40000);
    http.begin(this->api_url);
    http.addHeader("Content-Type", "application/json");
    Serial.println("Starting POST request...");
    
    int statusCode = http.POST(payload);
    
    if(statusCode > 0) {
        
        String response = http.getString();
        http.end();
        
        Serial.println("API Response (" + String(statusCode) + "): " + response);
        
        if(statusCode == 200) {
            JsonDocument respDoc;
            DeserializationError error = deserializeJson(respDoc, response);
            
            if(error) {
                Serial.println("JSON parsing error in response");
                return false;
            }
            
            String user_id = respDoc["user_id"].as<String>();
            String access_key = respDoc["access_key"].as<String>();
            
            Serial.println("USER ID    : " + user_id);
            Serial.println("Access Key : " + access_key);
            
            senderPref.begin("secret");
            senderPref.putString("user_id", user_id);
            senderPref.putString("access_key", access_key);
            senderPref.putBool("hasUser", true);
            senderPref.end();

            senderPref.begin("user_profile");
            senderPref.putString("firstname", dummyProfile.firstname);
            senderPref.putString("lastname", dummyProfile.lastname);
            senderPref.putString("middlename", dummyProfile.middlename);
            senderPref.putString("birthdate", dummyProfile.birthdate);
            senderPref.putString("em_contact", dummyProfile.emergency_contact);
            senderPref.putString("em_person", dummyProfile.emergency_person);
            senderPref.putString("region", dummyProfile.region);
            senderPref.putString("city_muni", dummyProfile.city_municipality);
            senderPref.putString("barangay", dummyProfile.barangay);
            senderPref.putString("contact", dummyProfile.contact);
            senderPref.end();
            
            applyPreferences();
        
            return true;
        }
        
        return statusCode >= 200 && statusCode < 300;
    }
    http.end();
    return false;
}

bool SenderProfile::isPingServerReachable() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Ping check skipped: WiFi not connected");
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(15);

    HTTPClient http;
    http.setTimeout(15000);
    http.begin(client, ping_url);

    Serial.println(ping_url);
    Serial.println(WiFi.dnsIP(0)); // Primary DNS
    Serial.println(WiFi.dnsIP(1)); // Secondary DNS
    
    int statusCode = http.GET();
    http.end();

    Serial.printf("Ping URL status: %d\n", statusCode);
    return statusCode == 200;
}

bool SenderProfile::sendEmergencyEvent(const GPSData &data) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Cannot send emergency event: WiFi not connected");
        return false;
    }

    JsonDocument doc;
    doc["device_id"] = data.device_id;
    doc["longitude"] = data.lon;
    doc["latitude"] = data.lat;
    doc["altitude"] = data.alt;
    doc["speed_kmph"] = data.spd;
    doc["ping_count"] = data.ping_count;
    doc["receiver_device_id"] = device_id;
    doc["emergency_id"] = data.emergency_id;
    doc["bounces"] = data.bounce;
    doc["is_click"] = data.isClick;
    doc["is_cancel"] = data.isCancellation;

    String payload;
    serializeJson(doc, payload);

    Serial.println("PAYLOAD : " + payload);

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(20);

    HTTPClient http;
    http.setTimeout(20000);
    http.begin(client, event_url);
    http.addHeader("Content-Type", "application/json");

    int statusCode = http.POST(payload);
    String response = statusCode > 0 ? http.getString() : "";
    http.end();

    Serial.printf("Event URL status: %d\n", statusCode);
    if (!response.isEmpty()) {
        Serial.println("Event response: " + response);
    }

    return statusCode >= 200 && statusCode < 300;
}

bool SenderProfile::sendEmergencyViaInternet(const GPSData &data) {
    // Internet path is allowed only when ping endpoint confirms reachability.
    if (!isPingServerReachable()) {
        return false;
    }

    return sendEmergencyEvent(data);
}