#ifndef SENDERPROFILE_H
#define SENDERPROFILE_H

#include <Preferences.h>

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
        String api_url = "https://erbriwan-api.onrender.com/device/registerUser"; 
        Profile myProfile;
        Preferences senderPref;
    public:
        SenderProfile();
        ~SenderProfile();

        bool checkExist();
        String toJsonString();
        void setSenderProfile(String firstname, String lastname, String middlename, String birthdate,
                            String emergency_contact, String emergency_person, String region,
                            String city_municipality, String barangay, String contact);
        Profile getProfile();

        void deleteProfile();
        void applyPreferences();
        void checkPreferences();

        bool uploadToAPI(String deviceId);
};

#endif

