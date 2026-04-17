#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "MyBle.h"

String dId = "ERBriwan_BLE";
static const char *serviceUuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char *rxCharacteristicUuid = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char *txCharacteristicUuid = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

BLECharacteristic *txCharacteristic = nullptr;
bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
	void onConnect(BLEServer *server) override {
		deviceConnected = true;
		Serial.println("BLE client connected");
	}

	void onDisconnect(BLEServer *server) override {
		deviceConnected = false;
		Serial.println("BLE client disconnected");
		server->getAdvertising()->start();
	}
};

class RxCallbacks : public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *characteristic) override {
		std::string value = characteristic->getValue();
		if (!value.empty()) {
			Serial.print("RX: ");
			Serial.println(value.c_str());
		}
	}
};

void MyBle::begin(String deviceId) {

    dId = deviceId;
	Serial.println("Starting BLE UART server");

	BLEDevice::init(dId.c_str());
	BLEServer *server = BLEDevice::createServer();
	server->setCallbacks(new ServerCallbacks());

	BLEService *service = server->createService(serviceUuid);
	txCharacteristic = service->createCharacteristic(
			txCharacteristicUuid,
			BLECharacteristic::PROPERTY_READ |
					BLECharacteristic::PROPERTY_NOTIFY);
	BLECharacteristic *rxCharacteristic = service->createCharacteristic(
			rxCharacteristicUuid,
			BLECharacteristic::PROPERTY_WRITE |
					BLECharacteristic::PROPERTY_WRITE_NR);

	txCharacteristic->addDescriptor(new BLE2902());
	txCharacteristic->setValue("Disconnected");
	rxCharacteristic->setCallbacks(new RxCallbacks());
	service->start();

	BLEAdvertising *advertising = BLEDevice::getAdvertising();
	advertising->addServiceUUID(serviceUuid);
	advertising->setScanResponse(true);
	advertising->setMinPreferred(0x06);
	advertising->setMinPreferred(0x12);
	BLEDevice::startAdvertising();

	Serial.println("BLE advertising started");
    return;
}

void MyBle::startBle() {
    BLEDevice::init(dId.c_str());

    return;
}

void MyBle::stopBle(){
    BLEDevice::deinit(true);

    return;
}