#ifndef LORA_DATA_HANDLER_H
#define LORA_DATA_HANDLER_H

#include <Arduino.h>
#include "../class/GPSData.h"
#include "../class/myLora/MyLora.h"
#include "../class/senderProfile/senderProfile.h"

extern MyLora lora;
extern void enqueueLoraSend(GPSData *dataPtr);
extern uint8_t max_bounce;

struct LastPingRecord {
    String deviceId;
    String emergencyId;
    int lastPingCount;
    bool used;
};

static const uint8_t MAX_TRACKED_EMERGENCIES = 40;
static LastPingRecord g_lastPingRecords[MAX_TRACKED_EMERGENCIES] = {};

/**
 * Returns true only for newer packets (strictly greater ping_count) per device+emergency.
 * If accepted, this function updates the stored ping_count.
 */
static bool shouldProcessLoraPacket(const GPSData &data) {
    int firstFreeIndex = -1;

    for (uint8_t i = 0; i < MAX_TRACKED_EMERGENCIES; ++i) {
        if (!g_lastPingRecords[i].used) {
            if (firstFreeIndex < 0) {
                firstFreeIndex = i;
            }
            continue;
        }

        if (g_lastPingRecords[i].deviceId == data.device_id &&
            g_lastPingRecords[i].emergencyId == data.emergency_id) {
            if (data.ping_count <= g_lastPingRecords[i].lastPingCount) {
                return false;
            }

            g_lastPingRecords[i].lastPingCount = data.ping_count;
            return true;
        }
    }

    const int recordIndex = (firstFreeIndex >= 0) ? firstFreeIndex : 0;
    g_lastPingRecords[recordIndex].used = true;
    g_lastPingRecords[recordIndex].deviceId = data.device_id;
    g_lastPingRecords[recordIndex].emergencyId = data.emergency_id;
    g_lastPingRecords[recordIndex].lastPingCount = data.ping_count;

    return true;
}

/**
 * Handle incoming LoRa GPSData packets from other senders
 * Call this periodically in the main loop to check and process received packets
 */
void handleLoraReceivedData() {
    GPSData receivedData;
    
    if (lora.receivePacketStruct(receivedData)) {
        if (!shouldProcessLoraPacket(receivedData)) {
            Serial.println("Ignored LoRa packet: ping_count is not newer for this device_id + emergency_id.");
            return;
        }

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

        bool sentViaInternet = SenderProfile::sendEmergencyViaInternet(receivedData);

        if (!sentViaInternet) {
            if(receivedData.bounce >= max_bounce){
                Serial.println("Max bounce count reached, dropping packet to avoid infinite loop.");
                return;
            }
            
            receivedData.bounce++;
            GPSData *ptr = new GPSData;
            *ptr = receivedData;
            enqueueLoraSend(ptr);
        }
    }
}

#endif
