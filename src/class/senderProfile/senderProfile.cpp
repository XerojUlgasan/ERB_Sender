#include "senderProfile.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

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

String SenderProfile::toJsonString() {
    String json = "{";
        json += "\"firstname\": \"" + myProfile.firstname + "\",";
        json += "\"lastname\": \"" + myProfile.lastname + "\",";
        json += "\"middlename\": \"" + myProfile.middlename + "\",";
        json += "\"birthdate\": \"" + myProfile.birthdate + "\",";
        json += "\"emergency_contact\": \"" + myProfile.emergency_contact + "\",";
        json += "\"emergency_person\": \"" + myProfile.emergency_person + "\",";
        json += "\"region\": \"" + myProfile.region + "\",";
        json += "\"city_municipality\": \"" + myProfile.city_municipality + "\",";
        json += "\"barangay\": \"" + myProfile.barangay + "\",";
        json += "\"contact\": \"" + myProfile.contact + "\"";
        json += "}";

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
    
    senderPref.begin("user_profile");

    senderPref.putString("firstname", firstname);
    senderPref.putString("lastname", lastname);
    senderPref.putString("middlename", middlename);
    senderPref.putString("birthdate", birthdate);
    senderPref.putString("em_contact", emergency_contact);
    senderPref.putString("em_person", emergency_person);
    senderPref.putString("region", region);
    senderPref.putString("city_muni", city_municipality);
    senderPref.putString("barangay", barangay);
    senderPref.putString("contact", contact);
    senderPref.end();

    return;
}

Profile SenderProfile::getProfile() {
    return myProfile;
}

void SenderProfile::deleteProfile() {
    this->senderPref.begin("user_profile");
    this->senderPref.clear();
    this->senderPref.end();
}

void SenderProfile::applyPreferences(){
    senderPref.begin("user_profile");

    myProfile.firstname = senderPref.getString("firstname", "");
    myProfile.lastname = senderPref.getString("lastname", "");
    myProfile.middlename = senderPref.getString("middlename", "");
    myProfile.birthdate = senderPref.getString("birthdate", "");
    myProfile.emergency_contact = senderPref.getString("em_contact", "");
    myProfile.emergency_person = senderPref.getString("em_person", "");
    myProfile.region = senderPref.getString("region", "");
    myProfile.city_municipality = senderPref.getString("city_muni", "");
    myProfile.barangay = senderPref.getString("barangay", "");
    myProfile.contact = senderPref.getString("contact", "");
    
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
    if(!SenderProfile::checkExist()){
        Serial.println("Error in user profile maybe lack of field or does not exists.");
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(15);  // 15 second timeout
    client.setHandshakeTimeout(15);  // SSL handshake timeout

    HTTPClient http;
    http.setTimeout(20000);

    if(!http.begin(client, api_url)){
        Serial.println("HTTP begin failed!");
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["user_fn"] = myProfile.firstname;
    doc["user_ln"] = myProfile.lastname;
    doc["birthdate"] = myProfile.birthdate;
    doc["region"] = myProfile.region;
    doc["city_minucipality"] = myProfile.city_municipality;
    doc["barangay"] = myProfile.barangay;
    doc["contact"] = myProfile.contact;
    doc["em_contact"] = myProfile.emergency_contact;
    doc["em_person"] = myProfile.emergency_person;
    doc["device_id"] = deviceId;

    if(myProfile.middlename.isEmpty()){
        doc["user_mn"] = nullptr;
    }else{
        doc["user_mn"] = myProfile.middlename;
    }

    String payload;
    serializeJson(doc, payload);

    int statusCode = http.POST(payload);
    if(statusCode <= 0){
        Serial.println("HTTP POST failed: " + String(statusCode));
        http.end();
        return false;
    }

    String response = http.getString();
    http.end();

    JsonDocument respDoc;
    DeserializationError error =  deserializeJson(respDoc, response);

    if(error) return false;
    else {
        String user_id = respDoc["user_id"].as<String>();
        String access_key = respDoc["user_id"].as<String>();

        Serial.println("USER ID    : " + user_id);
        Serial.println("Access Key : " + access_key);

        senderPref.begin("secret");
        senderPref.putString("user_id", user_id);
        senderPref.putString("access_key", access_key);
        senderPref.end();
    }

    Serial.println("API Response (" + String(statusCode) + "): " + response);

    return statusCode >= 200 && statusCode < 300;
}