#ifndef INITIALIZEPINS_H
#define INITIALIZEPINS_H

#include <Arduino.h>

#define button 32

#define wifi_led 34
#define webserver_led 35
#define gps_led 33
#define lora_s_led 25
#define lora_r_led 27

void initializePins() {
    
    pinMode(button, INPUT);

    pinMode(wifi_led, OUTPUT);
    pinMode(webserver_led, OUTPUT);
    pinMode(gps_led, OUTPUT);
    pinMode(lora_s_led, OUTPUT);
    pinMode(lora_r_led, OUTPUT);

    return;
}

#endif