#ifndef CLICKHANDLER_H
#define CLICKHANDLER_H

#include <Arduino.h>
#include "../class/myGps/MyGps.h"
#include "../class/myLora/MyLora.h"
#include "../class/GPSData.h"
#include "../initializePins.h"

extern MyGps gps;
extern MyLora lora;
extern const String device_id;
extern int ping_count;

// Asynchronous LoRa sending is handled by a dedicated task in main.cpp.
// This function enqueues a GPSData pointer for that task to send.
void enqueueLoraSend(GPSData *dataPtr);

// Button / click detection state (3 clicks within 5s)
bool lastBtnState = LOW;
unsigned long firstClickTime = 0;
uint8_t clickCount = 0;

// Long-press (10s) detection
unsigned long pressStartTime = 0;
bool longPressHandled = false;

// SOS ping session state
bool sosActive = false;                 // true after initial 3-click SOS
unsigned long lastFollowPingTime = 0;   // for periodic follow-up pings

// Cancellation state (10 packets, 5s apart)
bool cancellationActive = false;
unsigned long lastCancellationTime = 0;
int cancellationPacketsSent = 0;

// Adjust these two thresholds if your wiring/noise requires it (0..4095 ADC)
const int BUTTON_PRESS_THRESHOLD  = 3000; // >= 3000 ≈ HIGH (close to 4095)
const int BUTTON_RELEASE_THRESHOLD = 1000; // <= 1000 ≈ LOW  (close to 0)

// Timing constants (ms)
const unsigned long TRIPLE_CLICK_WINDOW   = 5000UL;   // 3 clicks within 5s
const unsigned long LONG_PRESS_DURATION   = 10000UL;  // 10s hold to cancel
const unsigned long FOLLOW_PING_INTERVAL  = 10000UL;  // gap between follow pings (10s)
const unsigned long CANCELLATION_INTERVAL = 10000UL;  // gap between cancellation packets (10s)
const int CANCELLATION_REPEAT_COUNT       = 15;       // send cancellation 15 times

// Helper to build GPSData and enqueue it for async LoRa sending
void enqueueGpsPacket(bool isClick, bool isCancellation) {
    GPSData temp = gps.getGPSDataStuct(device_id, ping_count, isClick, isCancellation);
    GPSData *ptr = new GPSData;
    *ptr = temp;
    enqueueLoraSend(ptr);
}

void clickHandler() {
    unsigned long now = millis();

    // ---- Periodic follow-up pings while SOS is active ----
    if (sosActive && !cancellationActive) {
        if (lastFollowPingTime == 0 || (now - lastFollowPingTime) >= FOLLOW_PING_INTERVAL) {
            enqueueGpsPacket(false, false); // following ping
            lastFollowPingTime = now;
        }
    }

    // ---- Cancellation packets (10x, 5s apart) ----
    if (cancellationActive) {
        if (cancellationPacketsSent < CANCELLATION_REPEAT_COUNT) {
            if (lastCancellationTime == 0 || (now - lastCancellationTime) >= CANCELLATION_INTERVAL) {
                enqueueGpsPacket(false, true); // cancellation ping

                cancellationPacketsSent++;
                lastCancellationTime = now;
            }
        } else {
            // Finished all cancellation packets; reset state so new SOS can be started
            cancellationActive = false;
            cancellationPacketsSent = 0;
            lastCancellationTime = 0;
            clickCount = 0;
            firstClickTime = 0;
            ping_count = 0; // reset ping counter after final cancellation ping
        }
    }

    // While SOS or cancellation is active, ignore new click sequences
    // (but we still monitor for long press to start cancellation when SOS is active).

    // ---- Read button as analog (0..4095) and convert to boolean with hysteresis ----
    int analogValue = analogRead(button); // 0..4095 on ESP32

    bool currBtnState = lastBtnState;
    if (!lastBtnState && analogValue >= BUTTON_PRESS_THRESHOLD) {
        currBtnState = HIGH;   // treat as pressed
    } else if (lastBtnState && analogValue <= BUTTON_RELEASE_THRESHOLD) {
        currBtnState = LOW;    // treat as released
    }

    // ---- Long-press detection (only meaningful when SOS is active) ----
    if (currBtnState) { // button currently pressed
        if (!lastBtnState) {
            // just pressed
            pressStartTime = now;
            longPressHandled = false;
        } else if (!longPressHandled && pressStartTime > 0 && (now - pressStartTime) >= LONG_PRESS_DURATION) {
            // 10s hold detected while SOS is active -> start cancellation
            if (sosActive && !cancellationActive) {
                Serial.println("Long press detected, starting cancellation...");
                sosActive = false;              // stop follow pings
                cancellationActive = true;
                cancellationPacketsSent = 0;
                lastCancellationTime = 0;      // send first cancellation on next loop
                longPressHandled = true;
            }
        }
    } else { // button currently released
        if (lastBtnState) {
            // just released
            pressStartTime = 0;
            longPressHandled = false;
        }
    }

    // ---- Triple-click detection (3 clicks in 5s) ----
    // Only when no SOS or cancellation is currently active.
    if (!sosActive && !cancellationActive) {
        // Detect rising edge: LOW -> HIGH
        if (currBtnState && !lastBtnState) {
            Serial.println("CLICK");

            if (clickCount == 0 || (now - firstClickTime) > TRIPLE_CLICK_WINDOW) {
                clickCount = 1;
                firstClickTime = now;
            } else {
                clickCount++;
            }

            if (clickCount >= 3 && (now - firstClickTime) <= TRIPLE_CLICK_WINDOW) {
                Serial.println("3 clicks detected! Starting SOS pings...");
                // Initial SOS packet (user-triggered)
                enqueueGpsPacket(true, false);

                // Enter SOS active state; follow-up pings will be sent periodically
                sosActive = true;
                lastFollowPingTime = now;

                // Reset click detection state
                clickCount = 0;
                firstClickTime = 0;
            }
        }
    }

    lastBtnState = currBtnState;
}

#endif