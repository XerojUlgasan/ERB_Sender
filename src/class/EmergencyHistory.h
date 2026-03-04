#ifndef EMERGENCYHISTORY_H
#define EMERGENCYHISTORY_H

#include <Arduino.h>
#include <Preferences.h>

// Emergency record struct (fixed size for serialization)
#pragma pack(push, 1)
struct EmergencyRecord {
    char emergency_id[6];  // 5 chars + null terminator
    float lon;
    float lat;
    bool isStart;
};
#pragma pack(pop)

// Maximum number of emergency records to store
const int MAX_EMERGENCY_RECORDS = 20;
const char *EMERGENCY_HISTORY_KEY = "emergencyHist";

/**
 * Generate a random 5-character alphanumeric emergency ID
 * @return String containing random ID (e.g., "A3K9X")
 */
String generateEmergencyId() {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    String id = "";
    for (int i = 0; i < 5; i++) {
        id += charset[random(0, sizeof(charset) - 1)];
    }
    return id;
}

/**
 * Record an emergency event (start or end)
 * Automatically removes oldest entry if array is full (max 20 entries)
 * 
 * @param pref Preferences object (must be in begin state)
 * @param lon Longitude coordinate
 * @param lat Latitude coordinate
 * @param emergency_id 5-character emergency ID
 * @param isStart true for SOS start, false for cancellation/end
 */
void recordEmergency(Preferences &pref, float lon, float lat, const String &emergency_id, bool isStart) {
    // Get current history from preferences
    size_t len = pref.getBytesLength(EMERGENCY_HISTORY_KEY);
    int recordCount = len / sizeof(EmergencyRecord);
    
    // If array is full (20 records), we need to remove oldest
    if (recordCount >= MAX_EMERGENCY_RECORDS) {
        // Read all records except the first one (oldest)
        EmergencyRecord records[MAX_EMERGENCY_RECORDS];
        pref.getBytes(EMERGENCY_HISTORY_KEY, records, len);
        
        // Shift array left (remove oldest, keep newest 19)
        for (int i = 0; i < MAX_EMERGENCY_RECORDS - 1; i++) {
            records[i] = records[i + 1];
        }
        recordCount = MAX_EMERGENCY_RECORDS - 1;
        
        // Write back the shifted array
        pref.putBytes(EMERGENCY_HISTORY_KEY, records, recordCount * sizeof(EmergencyRecord));
    } else {
        // Read existing records if any
        EmergencyRecord records[MAX_EMERGENCY_RECORDS];
        if (len > 0) {
            pref.getBytes(EMERGENCY_HISTORY_KEY, records, len);
        }
        
        // Add new record at the end
        EmergencyRecord newRecord;
        memset(newRecord.emergency_id, 0, sizeof(newRecord.emergency_id));
        strncpy(newRecord.emergency_id, emergency_id.c_str(), 5);
        newRecord.lon = lon;
        newRecord.lat = lat;
        newRecord.isStart = isStart;
        
        records[recordCount] = newRecord;
        recordCount++;
        
        // Write all records back
        pref.putBytes(EMERGENCY_HISTORY_KEY, records, recordCount * sizeof(EmergencyRecord));
    }
}

/**
 * Insert or update an emergency record
 * Same as recordEmergency for now (inserts new record)
 * 
 * @param pref Preferences object (must be in begin state)
 * @param lon Longitude coordinate
 * @param lat Latitude coordinate
 * @param emergency_id 5-character emergency ID
 * @param isStart true for SOS start, false for cancellation/end
 */
void insertOrUpdateEmergency(Preferences &pref, float lon, float lat, const String &emergency_id, bool isStart) {
    recordEmergency(pref, lon, lat, emergency_id, isStart);
}

/**
 * Retrieve all emergency history records
 * 
 * @param pref Preferences object (must be in begin state)
 * @param outRecords Pointer to array to store records
 * @param maxRecords Maximum number of records that outRecords can hold
 * @return Number of records retrieved
 */
int getAllEmergencyHistory(Preferences &pref, EmergencyRecord *outRecords, int maxRecords) {
    if (outRecords == nullptr || maxRecords <= 0) {
        return 0;
    }
    
    size_t len = pref.getBytesLength(EMERGENCY_HISTORY_KEY);
    int recordCount = len / sizeof(EmergencyRecord);
    
    if (recordCount <= 0) {
        return 0;
    }
    
    // Limit to max records we can store
    recordCount = min(recordCount, maxRecords);
    
    pref.getBytes(EMERGENCY_HISTORY_KEY, outRecords, recordCount * sizeof(EmergencyRecord));
    
    return recordCount;
}

/**
 * Delete all emergency history records
 * 
 * @param pref Preferences object (must be in begin state)
 */
void deleteAllEmergencyHistory(Preferences &pref) {
    pref.remove(EMERGENCY_HISTORY_KEY);
}

/**
 * Delete emergency records by emergency_id
 * 
 * @param pref Preferences object (must be in begin state)
 * @param emergency_id The emergency ID to delete
 * @return Number of records deleted
 */
int deleteEmergencyById(Preferences &pref, const String &emergency_id) {
    size_t len = pref.getBytesLength(EMERGENCY_HISTORY_KEY);
    int recordCount = len / sizeof(EmergencyRecord);
    
    if (recordCount <= 0) {
        return 0;
    }
    
    EmergencyRecord records[MAX_EMERGENCY_RECORDS];
    pref.getBytes(EMERGENCY_HISTORY_KEY, records, len);
    
    // Filter out records with matching emergency_id
    int newCount = 0;
    int deletedCount = 0;
    for (int i = 0; i < recordCount; i++) {
        if (strcmp(records[i].emergency_id, emergency_id.c_str()) != 0) {
            records[newCount] = records[i];
            newCount++;
        } else {
            deletedCount++;
        }
    }
    
    // Write back filtered records
    if (newCount > 0) {
        pref.putBytes(EMERGENCY_HISTORY_KEY, records, newCount * sizeof(EmergencyRecord));
    } else {
        pref.remove(EMERGENCY_HISTORY_KEY);
    }
    
    return deletedCount;
}

/**
 * Print all emergency history records (for debugging)
 * 
 * @param pref Preferences object (must be in begin state)
 */
void printEmergencyHistory(Preferences &pref) {
    EmergencyRecord records[MAX_EMERGENCY_RECORDS];
    int count = getAllEmergencyHistory(pref, records, MAX_EMERGENCY_RECORDS);
    
    Serial.println("=== Emergency History ===");
    Serial.printf("Total records: %d\n", count);
    
    for (int i = 0; i < count; i++) {
        Serial.printf("[%d] ID: %s | Lon: %.6f | Lat: %.6f | Start: %s\n",
            i + 1,
            records[i].emergency_id,
            records[i].lon,
            records[i].lat,
            records[i].isStart ? "Yes" : "No"
        );
    }
    Serial.println("========================");
}

#endif
