#include <Arduino.h>
#include "Tone32.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Preferences.h>
#include <Ticker.h>
//68ADAB62-25D1-24B5-F68C-A11C0C65B3E7
//4F29C692-7901-DAC3-08E1-7508A761A154
int scanTime = 2; //In seconds
BLEScan *pBLEScan;
#define SERVICE_UUID        "ebe000da-bbbb-cccc-dddd-eeeeffff0000"
#define CHARACTERISTIC_UUID "ebe01234-6666-7777-8888-9999aaaabbbb"
BLEUUID targetService = BLEUUID(SERVICE_UUID);
bool deviceConnected = false;
unsigned long findings[8] = {0,0,0,0,0,0,0,0};
Preferences rom;

unsigned toneVal = 0;
Ticker timer;
unsigned cycle = 0;

void save() {
  rom.putBytes("friendAddress", findings, sizeof(findings));
}

void timerHandler() {
  ++cycle;
  if(cycle > 3) {cycle = 0;}
  if(!toneVal) {return;}
  if(!cycle%2) {return;}

  int freq = toneVal * 400;
  if(cycle == 3) {tone(14, freq, 200, 0);}
  else if(toneVal > 2) {tone(14, freq, 200, 0);}
}

void setVibrate(int rssi) { //-95 ~ -75
  if(rssi > -95) {
    if(rssi > -85) {
      if(rssi > -75) {//near
        toneVal = 3;
      } else {//mid
        toneVal = 2;
      }
    } else {//far
      toneVal = 1;
    }
  } else {//too far
    toneVal = 0;
  }
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      //Serial.println(advertisedDevice.getAddress().toString().c_str());
      //if(advertisedDevice.getAddress().toString().c_str()=="04:72:95:E8:32:C7") {
        //Serial.println("detected targett.");
      //}
      if(advertisedDevice.haveServiceUUID()) {
        if(advertisedDevice.getServiceUUID().equals(targetService)) {
          if(advertisedDevice.haveRSSI()) {
            //Serial.println(advertisedDevice.getRSSI()));
            setVibrate(advertisedDevice.getRSSI());
          }
          //Serial.println("detected service");
        }
      }
      //advertisedDevice.getRSSI();
    }
};

class BLEServerEventHandler: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class BCDelegate: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {Serial.print(rxValue[i]);}
      Serial.println();
      // Do stuff based on the command received from the app
      if (rxValue.find("A") != -1) { 
        Serial.print("Turning ON!");
      }
      Serial.println();
      Serial.println("*********");
    }
  }
};

void setup() {
  tone(14, 500, 80, 0);
  Serial.begin(115200);
  Serial.println("Scanning...");
  BLEDevice::init("ESP32 BLE Test Device");

  //EEPROM
  //rom.begin("friendList");
  //size_t length = rom.getBytesLength("friendAddress");
  //if(length) {
    //rom.getBytes("friendAddress", findings, length);
  //}

  //Peers
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  //Phone
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEServerEventHandler());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new BCDelegate());
  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined");
  tone(14, 1000, 500, 0);
  timer.attach_ms(250, timerHandler);
}

void loop() {
  toneVal = 0;
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.println(foundDevices.getCount());
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(30);
}

/*
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32 BLE Test Device");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEServerEventHandler());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new BCDelegate());
  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}
*/