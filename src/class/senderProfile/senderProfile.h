#ifndef SENDERPROFILE_H
#define SENDERPROFILE_H

#include <Preferences.h>
#include "../GPSData.h"

struct Profile {
    String firstname;
    String lastname;
    String middlename;
    String birthdate;
    String emergency_contact;
    String emergency_person;
    String region;
    String city_municipality;
    String barangay;
    String contact;
};

class SenderProfile
{
    private:
        Profile myProfile;
        Preferences senderPref;
    public:
        static Profile dummyProfile;
        static String device_id;
        static String api_url;
        static String ping_url;
        static String event_url;
        SenderProfile();
        ~SenderProfile();

        bool checkExist();
        String toJsonString(const String &deviceId = "");
        void setSenderProfile(String firstname, String lastname, String middlename, String birthdate,
                            String emergency_contact, String emergency_person, String region,
                            String city_municipality, String barangay, String contact);
        Profile getProfile();

        void deleteProfile();
        void applyPreferences();
        void checkPreferences();

        bool uploadToAPI(String deviceId);
        static bool isPingServerReachable();
        static bool sendEmergencyEvent(const GPSData &data);
        static bool sendEmergencyViaInternet(const GPSData &data);
};

#endif