#include "senderProfile.h"

SenderProfile::SenderProfile(Preferences& pref)
{
    pref.begin("user_profile");

    fullname                  = pref.getString("fullname", "");
    address                   = pref.getString("address", "");
    contact_number            = pref.getString("contact_number", "");
    emergency_contact_person  = pref.getString("emergency_contact_person", "");
    emergency_contact_number  = pref.getString("emergency_contact_number", "");
    
    pref.end();
}

SenderProfile::~SenderProfile()
{
    Serial.println("Sender Profile with " + fullname + " is destroyed!");
}

bool SenderProfile::checkExist() {
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