#include <Arduino.h>

class MyLora
{
private:

public:
    MyLora(int nss, int rst, int dio);
    ~MyLora();

    void sendPacket(String message);
    void startReceive();
    void stopReceive();
};
