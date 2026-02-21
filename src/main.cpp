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

void loraTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    GPSData *dataPtr = nullptr;
    if (xQueueReceive(loraQueue, &dataPtr, portMAX_DELAY) == pdTRUE) {
      if (dataPtr != nullptr) {
        lora.sendPacketStruct(*dataPtr);
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

// Function to synchronize system time with NTP server
void syncSystemTime() {
  Serial.println("Synchronizing system time with NTP...");
  
  // Configure time with NTP servers
  configTime(0, 0, "pool.ntp.org", "time.nist.gov", "time.cloudflare.com");
  
  Serial.println("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  int attempts = 0;
  
  while(now < 24 * 3600 && attempts < 20) {  // Wait for valid time (later than 1970)
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    attempts++;
  }
  
  Serial.println();
  struct tm timeinfo = *localtime(&now);
  Serial.print("Current time: ");
  Serial.println(asctime(&timeinfo));
}

void setup() {
  esp_task_wdt_init(15, true);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(device_id, "Malopit123");
  Serial.begin(115200);

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

  // Create queue and LoRa sender task
  loraQueue = xQueueCreate(10, sizeof(GPSData *));
  if (loraQueue != nullptr) {
    xTaskCreatePinnedToCore(
      loraTask,
      "LoRaTask",
      4096,
      nullptr,
      1,
      nullptr,
      1  // run on core 1 to keep core 0 freer
    );
  }
}

#include "./helpers/clickHandler.h"

void loop() {
  pref.begin("secret");
  isRegistered = pref.getBool("hasUser");

  if(isRegistered) clickHandler(); // ONLY ALLOW IF A USER IS REGISTERED TO THIS DEVICE

  pref.end();
  // clickHandler();
}
