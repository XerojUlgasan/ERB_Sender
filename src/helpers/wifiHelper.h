#ifndef WIFIHELPER_H
#define WIFIHELPER_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

void detectNetworks(std::vector<String> &ssid, std::vector<bool> &isSecure) {
    delay(100);

    int n = WiFi.scanNetworks(false, true); // false = not async, true = show hidden networks
    // Note: We use sync mode but with proper timeout handling in the web handler

    if(n == 0){
        Serial.println("No Network Detected");
    }else{
        Serial.println((String)n + " Network Detected");

        for(int i = 0; i < n; i++){
            ssid.push_back(WiFi.SSID(i));
            isSecure.push_back((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? false : true);

            Serial.println("\nNetwork  : " + WiFi.SSID(i));
            Serial.println("Security : " + String((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured"));
        }
    }

    WiFi.scanDelete();

    return;
}

#endif