#include <Preferences.h>

class SenderProfile
{
    private:
        String fullname, address, contact_number, emergency_contact_person, emergency_contact_number;
    public:
        SenderProfile(Preferences& pref);
        ~SenderProfile();

        bool checkExist();
        String toJsonString();
};