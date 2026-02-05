#include <Preferences.h>

class SenderProfile
{
    private:
    public:
        static String fullname, address, contact_number, emergency_contact_person, emergency_contact_number;
        Preferences senderPref;

        SenderProfile();
        ~SenderProfile();

        bool checkExist();
        String toJsonString();
        void setSenderProfile(String fn, String add, String con_num, String em_con_per, String em_con_num);

        void applyPreferences();
        void checkPreferences();
};