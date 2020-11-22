//MARK: Runtime
#include <Arduino.h>
#include <Ticker.h>
#include "Tone32.h"
unsigned toneVal = 0;
unsigned cycle = 0;
Ticker timer;

//MARK: ESP32 Bluetooth
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#define SERVICE_UUID        "ebe000da-bbbb-cccc-dddd-eeeeffff0000"
#define CHARACTERISTIC_UUID "ebe01234-6666-7777-8888-9999aaaabbbb"
int scanTime = 2; //In seconds
BLEScan *pBLEScan;
BLEUUID targetService = BLEUUID(SERVICE_UUID);
bool deviceConnected = false;

//MARK: ROM Access
// #include <Preferences.h>
// unsigned long findings[8] = {0,0,0,0,0,0,0,0};
// Preferences rom;
// void save() {
//   rom.putBytes("friendAddress", findings, sizeof(findings));
// }

//MARK: OLED Display
#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, 3, 2);//sda scl

//MARK: Timer Interrupt
void timerHandler() {
  ++cycle; //1 sec as a whole cycle |0|1|2|3|
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
      if(advertisedDevice.haveServiceUUID()) {
        if(advertisedDevice.getServiceUUID().equals(targetService)) {
          if(advertisedDevice.haveRSSI()) {
            setVibrate(advertisedDevice.getRSSI());
          }
        }
      }
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
      for (int i = 0; i < rxValue.length(); i++) {Serial.print(rxValue[i]);}
      Serial.println();
      //rxValue
    }
  }
};

void setup() {
  tone(14, 500, 80, 0);
  Serial.begin(115200);
  Serial.println("Scanning...");
  
  //Display
  display.clear();
  display.print("hello");

  //EEPROM
  //rom.begin("friendList");
  //size_t length = rom.getBytesLength("friendAddress");
  //if(length) {
    //rom.getBytes("friendAddress", findings, length);
  //}

  //Peers
  BLEDevice::init("ESP32 BLE Test Device");
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
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new BCDelegate());
  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  //enabling iPhone connections
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
  pBLEScan->clearResults();   //release memory
  delay(30);
}
