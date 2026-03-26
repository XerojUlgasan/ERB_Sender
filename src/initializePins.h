#ifndef INITIALIZEPINS_H
#define INITIALIZEPINS_H

#include <Arduino.h>

#define button 32

#define wifi_led 13
#define webserver_led 12
#define toggle_web_server 22
#define gps_led 33
#define lora_s_led 25
#define lora_r_led 27

void initializePins() {
    
    pinMode(button, INPUT);
    pinMode(toggle_web_server, INPUT_PULLDOWN);

    pinMode(wifi_led, OUTPUT);
    pinMode(webserver_led, OUTPUT);

    pinMode(gps_led, OUTPUT);
    pinMode(lora_s_led, OUTPUT);
    pinMode(lora_r_led, OUTPUT);

    return;
}

#endif