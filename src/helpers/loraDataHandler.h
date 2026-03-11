#ifndef LORA_DATA_HANDLER_H
#define LORA_DATA_HANDLER_H

#include <Arduino.h>
#include "../class/GPSData.h"
#include "../class/myLora/MyLora.h"
#include "../class/senderProfile/senderProfile.h"

extern MyLora lora;
extern void enqueueLoraSend(GPSData *dataPtr);

/**
 * Handle incoming LoRa GPSData packets from other senders
 * Call this periodically in the main loop to check and process received packets
 */
void handleLoraReceivedData() {
    GPSData receivedData;
    
    if (lora.receivePacketStruct(receivedData)) {

        Serial.println("\n========== RECEIVED LoRa DATA ==========");
        Serial.print("Device ID: ");
        Serial.println(receivedData.device_id);
        Serial.print("Location: (");
        Serial.print(receivedData.lat, 6);
        Serial.print(", ");
        Serial.print(receivedData.lon, 6);
        Serial.println(")");
        Serial.print("Altitude: ");
        Serial.print(receivedData.alt);
        Serial.println(" m");
        Serial.print("Speed: ");
        Serial.print(receivedData.spd);
        Serial.println(" km/h");
        Serial.print("Ping Count: ");
        Serial.println(receivedData.ping_count);
        Serial.print("Emergency ID: ");
        Serial.println(receivedData.emergency_id);
        Serial.print("Bounces: ");
        Serial.println(receivedData.bounce);
        Serial.print("Is Click: ");
        Serial.println(receivedData.isClick ? "Yes" : "No");
        Serial.print("Is Cancellation: ");
        Serial.println(receivedData.isCancellation ? "Yes" : "No");
        Serial.print("Location Valid: ");
        Serial.println(receivedData.isLocValid ? "Yes" : "No");
        Serial.print("Altitude Valid: ");
        Serial.println(receivedData.isAltValid ? "Yes" : "No");
        Serial.print("Speed Valid: ");
        Serial.println(receivedData.isSpdValid ? "Yes" : "No");
        Serial.println("=========================================\n");

        receivedData.bounce++;
        bool sentViaInternet = SenderProfile::sendEmergencyViaInternet(receivedData);
        if (!sentViaInternet) {
            GPSData *ptr = new GPSData;
            *ptr = receivedData;
            enqueueLoraSend(ptr);
        }
    }
}

#endif
