#include <Arduino.h>

class MyLora
{
private:
    int _nss, _rst, _dio;
public:
    MyLora(int nss, int rst, int dio);
    ~MyLora();

    void begin();
    void sendPacket(String message);
    void startReceive();
    void stopReceive();
};
