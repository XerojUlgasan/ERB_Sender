#include "senderProfile.h"

String SenderProfile::fullname = "";
String SenderProfile::address = "";
String SenderProfile::contact_number = "";
String SenderProfile::emergency_contact_person = "";
String SenderProfile::emergency_contact_number = "";

SenderProfile::SenderProfile()
{
    applyPreferences();
}

SenderProfile::~SenderProfile()
{
    Serial.println("Sender Profile with " + fullname + " is destroyed!");
}

bool SenderProfile::checkExist() {
    applyPreferences();

    if(fullname.isEmpty() || address.isEmpty() || contact_number.isEmpty() || emergency_contact_number.isEmpty() || emergency_contact_person.isEmpty()){
        return false;
    }else{
        return true;
    }
}

String SenderProfile::toJsonString() {
    String json = "{";
        json += "\"fullname\": \"" + fullname + "\",";
        json += "\"address\": \"" + address + "\",";
        json += "\"contact_number\": \"" + contact_number + "\",";
        json += "\"emergency_contact_person\": \"" + emergency_contact_person + "\",";
        json += "\"emergency_contact_number\": \"" + emergency_contact_number + "\"";
        json += "}";

    return json;
}

void SenderProfile::setSenderProfile(String fn, String add, String con_num, String em_con_per, String em_con_num) {
    fullname = fn;
    address = add;
    contact_number = con_num;
    emergency_contact_person = em_con_per;
    emergency_contact_number = em_con_num;
    
    senderPref.begin("user_profile");

    senderPref.putString("fullname", fn);
    senderPref.putString("address", add);
    senderPref.putString("contact_number", con_num);
    senderPref.putString("em_con_person", em_con_per);
    senderPref.putString("em_con_number", em_con_num);

    senderPref.end();

    return;
}

void SenderProfile::deleteProfile() {
    this->senderPref.begin("user_profile");
    this->senderPref.clear();
    this->senderPref.end();
}

void SenderProfile::applyPreferences(){
    senderPref.begin("user_profile");

    fullname                  = senderPref.getString("fullname", "");
    address                   = senderPref.getString("address", "");
    contact_number            = senderPref.getString("contact_number", "");
    emergency_contact_person  = senderPref.getString("em_con_person", "");
    emergency_contact_number  = senderPref.getString("em_con_number", "");
    
    senderPref.end();

    return;
}

void SenderProfile::checkPreferences(){
    senderPref.begin("user_profile");

    Serial.println(senderPref.getString("fullname", ""));
    Serial.println(senderPref.getString("address", ""));
    Serial.println(senderPref.getString("contact_number", ""));
    Serial.println(senderPref.getString("em_con_person", ""));
    Serial.println(senderPref.getString("em_con_number", ""));

    senderPref.end();
    return;
}