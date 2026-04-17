#ifndef INITIALIZEPINS_H
#define INITIALIZEPINS_H

#include <Arduino.h>

#define button 32
#define toggle_web_server 22

#define webserver_led 12 //RED
#define wifi_led 13 // GREEN
#define gps_led 33 // GREEN
#define lora_s_led 25 // YELLOW
#define lora_r_led 27 //YELLOW

void initializePins() {
    
    pinMode(button, INPUT_PULLDOWN);
    pinMode(toggle_web_server, INPUT_PULLDOWN);

    pinMode(wifi_led, OUTPUT);
    pinMode(webserver_led, OUTPUT);

    pinMode(gps_led, OUTPUT);
    pinMode(lora_s_led, OUTPUT);
    pinMode(lora_r_led, OUTPUT);

    return;
}

#endif