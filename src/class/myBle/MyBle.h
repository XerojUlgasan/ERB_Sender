#ifndef MYBLE_H
#define MYBLE_H
#include <Arduino.h>

class MyBle
{
private:
    
public:
    void begin(String deviceId);
    static void stopBle();
    static void startBle();
};

#endif