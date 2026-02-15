#ifndef INITIALIZEPINS_H
#define INITIALIZEPINS_H

#include <Arduino.h>

#define button 32

void initializePins() {
    
    pinMode(button, INPUT);

    return;
}

#endif