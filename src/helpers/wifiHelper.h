#ifndef WIFIHELPER_H
#define WIFIHELPER_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

void detectNetworks(std::vector<String> &ssid, std::vector<bool> &isSecure) {
    // Ensure WiFi is in proper state for scanning
    // WiFi.disconnect();
    

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


bool autoConnectToSavedNetworks(Preferences &pref) {
    Serial.println("Attempting to auto-connect to saved networks...");
    
    // Read saved networks from preferences
    pref.begin("secret", true);
    String storedNetworks = pref.getString("saved_networks", "[]");
    pref.end();
    
    JsonDocument savedDoc;
    DeserializationError parseError = deserializeJson(savedDoc, storedNetworks);
    
    if (parseError || !savedDoc.is<JsonArray>()) {
        Serial.println("No saved networks found or invalid format");
        return false;
    }
    
    JsonArray savedNetworks = savedDoc.as<JsonArray>();
    
    if (savedNetworks.size() == 0) {
        Serial.println("No saved networks available");
        return false;
    }
    
    // Scan for available networks
    Serial.println("Scanning for available networks...");
    WiFi.disconnect();
    delay(100);
    
    int n = WiFi.scanNetworks(false, true);
    
    if (n == 0) {
        Serial.println("No networks detected in scan");
        WiFi.scanDelete();
        return false;
    }
    
    Serial.printf("Found %d networks in scan\n", n);
    
    // Try to connect to saved networks in priority order
    for (JsonObject network : savedNetworks) {
        String savedSsid = network["ssid"].as<String>();
        String savedPassword = network["password"].as<String>();
        
        // Check if this saved network is available
        for (int i = 0; i < n; i++) {
            if (WiFi.SSID(i) == savedSsid) {
                Serial.printf("Found saved network: %s, attempting to connect...\n", savedSsid.c_str());
                
                WiFi.begin(savedSsid, savedPassword);
                
                int timeout = 10000;
                int elapsed = 0;
                
                while (WiFi.status() != WL_CONNECTED && elapsed < timeout) {
                    delay(100);
                    elapsed += 100;
                    Serial.print(".");
                }
                Serial.println();
                
                if (WiFi.status() == WL_CONNECTED) {
                    Serial.printf("Successfully connected to %s\n", savedSsid.c_str());
                    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
                    WiFi.scanDelete();
                    return true;
                } else {
                    Serial.printf("Failed to connect to %s\n", savedSsid.c_str());
                    WiFi.disconnect(true);
                    delay(100);
                }
                
                break; // Found the network in scan, no need to continue inner loop
            }
        }
    }
    
    WiFi.scanDelete();
    Serial.println("Could not connect to any saved network");
    return false;
}

// FreeRTOS task for automatic WiFi connection
void wifiAutoConnectTask(void *pvParameters) {
    Preferences *pref = (Preferences *)pvParameters;
    
    // Initial delay to allow system to stabilize
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // Try initial connection
    Serial.println("[WiFi Task] Starting auto-connect task");
    autoConnectToSavedNetworks(*pref);
    
    for (;;) {
        // Check WiFi status every 30 seconds
        vTaskDelay(pdMS_TO_TICKS(30000));
        
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WiFi Task] WiFi disconnected, attempting reconnect...");
            autoConnectToSavedNetworks(*pref);
        } else {
            Serial.printf("[WiFi Task] WiFi connected to %s (IP: %s)\n", 
                         WiFi.SSID().c_str(), 
                         WiFi.localIP().toString().c_str());
        }
    }
}

// Function to start the WiFi auto-connect task
void startWifiAutoConnect(Preferences &pref) {
    xTaskCreatePinnedToCore(
        wifiAutoConnectTask,
        "WiFiAutoConnect",
        4096,
        &pref,
        1,
        nullptr,
        0  // run on core 0
    );
    Serial.println("[WiFi Task] Auto-connect task created");
}
#endif