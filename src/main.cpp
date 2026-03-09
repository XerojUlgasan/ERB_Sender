#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <time.h>
#include <sys/time.h>

#include "./class/GPSData.h"

#include "./helpers/wifiHelper.h"
#include "webServerHandler.h"

#include "./class/myGps/MyGps.h"
#include "class/myLora/MyLora.h"


//TODO : Proper sanitation in Senderprofile.setProfile
//TODO : Save wifi networks
//TODO : History endpoint
//TODO : Encryption key generation
//TODO : Encryption key and user recording to cloud database

const bool deviceIsSender = true;
const String device_id = "ERBriwan-001";
bool isRegistered= false;
int ping_count = 0;

Preferences pref;
MyGps gps;
MyLora lora(5, 14, 26);

// Queue and task for asynchronous LoRa sending
QueueHandle_t loraQueue = nullptr;

struct RelayTracker {
  bool used;
  String senderDeviceId;
  String emergencyId;
  int lastPingCount;
};

RelayTracker relayTrackers[30];

bool shouldRelayPacket(const GPSData &data) {
  if (data.device_id.isEmpty() || data.emergency_id.isEmpty()) {
    return false;
  }

  for (int i = 0; i < 30; i++) {
    if (relayTrackers[i].used &&
        relayTrackers[i].senderDeviceId == data.device_id &&
        relayTrackers[i].emergencyId == data.emergency_id) {
      if (data.ping_count > relayTrackers[i].lastPingCount) {
        relayTrackers[i].lastPingCount = data.ping_count;
        return true;
      }
      return false;
    }
  }

  for (int i = 0; i < 30; i++) {
    if (!relayTrackers[i].used) {
      relayTrackers[i].used = true;
      relayTrackers[i].senderDeviceId = data.device_id;
      relayTrackers[i].emergencyId = data.emergency_id;
      relayTrackers[i].lastPingCount = data.ping_count;
      return true;
    }
  }

  // Table full: overwrite oldest slot (simple rotating behavior).
  relayTrackers[0].used = true;
  relayTrackers[0].senderDeviceId = data.device_id;
  relayTrackers[0].emergencyId = data.emergency_id;
  relayTrackers[0].lastPingCount = data.ping_count;
  return true;
}

void loraTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    GPSData *dataPtr = nullptr;
    if (xQueueReceive(loraQueue, &dataPtr, portMAX_DELAY) == pdTRUE) {
      if (dataPtr != nullptr) {
        lora.sendPacketStruct(*dataPtr);
        lora.startReceive();
        delete dataPtr;
        // Enforce at least 10 seconds between any two LoRa packets
        vTaskDelay(pdMS_TO_TICKS(10000));
      }
    }

    // Short yield in case we woke up without work (defensive)
    vTaskDelay(1);
  }
}

void enqueueLoraSend(GPSData *dataPtr) {
  if (loraQueue == nullptr || dataPtr == nullptr) {
    if (dataPtr != nullptr) {
      delete dataPtr; // avoid leak if queue not ready
    }
    return;
  }

  // Non-blocking send; drop packet if queue is full
  if (xQueueSend(loraQueue, &dataPtr, 0) != pdTRUE) {
    delete dataPtr;
  }
}

void sendDummyLoraData() {
  GPSData dummy;
  dummy.lon = 121.0123;
  dummy.lat = 14.5678;
  dummy.alt = 50.0;
  dummy.spd = 5.5;
  dummy.device_id = "TEST-999";
  dummy.emergency_id = "DUM99";
  dummy.ping_count = 1;
  dummy.isClick = true;
  dummy.isCancellation = false;
  dummy.isLocValid = true;
  dummy.isAltValid = true;
  dummy.isSpdValid = true;
  
  Serial.println("Sending dummy LoRa data...");
  lora.sendPacketStruct(dummy);
  lora.startReceive();
  Serial.println("Dummy data sent!");
}

void loraRelayTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    GPSData receivedData;
    if (!lora.consumeReceivedPacket(receivedData)) {
      vTaskDelay(pdMS_TO_TICKS(50));
      continue;
    }

    // Ignore self packets to reduce local echo loops.
    if (receivedData.device_id == device_id) {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }

    if (!shouldRelayPacket(receivedData)) {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }

    bool uploaded = sender.sendEmergencyViaInternet(receivedData, device_id);
    if (!uploaded) {
      // Internet path unavailable; repeat the same packet over LoRa.
      lora.sendPacketStruct(receivedData);
      lora.startReceive();
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  esp_task_wdt_init(15, true);

  WiFi.mode(WIFI_AP_STA);
  
  // WiFi.begin("Ulgasan", "XerojHaha123?");
  WiFi.softAP(device_id, "Malopit123");
  Serial.begin(115200);
  
  // Start WiFi auto-connect task (runs in background)
  startWifiAutoConnect(pref);

  if(initializeWebServer(deviceIsSender, pref)){
    Serial.println("Setup Done!");
  }else{
    Serial.println("Setup Failed!");
    ESP.restart();
  } 
  Serial.println("ASDWADASD");
  
  gps.begin();
  lora.begin();
  lora.startReceive();

  // // Create queue and LoRa sender task
  // loraQueue = xQueueCreate(10, sizeof(GPSData *));
  // if (loraQueue != nullptr) {
  //   xTaskCreatePinnedToCore(
  //     loraTask,
  //     "LoRaTask",
  //     4096,
  //     nullptr,
  //     1,
  //     nullptr,
  //     1  // run on core 1 to keep core 0 freer
  //   );
  // }

  // xTaskCreatePinnedToCore(
  //   loraRelayTask,
  //   "LoRaRelayTask",
  //   6144,
  //   nullptr,
  //   1,
  //   nullptr,
  //   0
  // );


  delay(500);
  Serial.println(WiFi.localIP());
}

#include "./helpers/clickHandler.h"

void loop() {
  // pref.begin("secret");
  // isRegistered = pref.getBool("hasUser");

  // if(isRegistered) clickHandler(); // ONLY ALLOW IF A USER IS REGISTERED TO THIS DEVICE

  // pref.end();
  
  // // Continuously read GPS data to keep TinyGPS++ buffer updated
  // // gps.getLocation();
  
  // delay(100);  // Small delay to prevent tight loop
  // // clickHandler();

  sendDummyLoraData();
}
