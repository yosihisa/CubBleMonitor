/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "90af54ef-d367-41ec-ac04-b3f9f42cdcb2"
#define CHARACTERISTIC_UUID "a018d2c9-5838-4196-bd97-d00547de2c57"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Disonnected");
    }
};


void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // Create the BLE Device
  BLEDevice::init("JA07_Monitor_ESP32");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY 
                    );

  pCharacteristic->setValue("Hello World");

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start the service
  pServer->getAdvertising()->start();
  BLEDevice::startAdvertising();


  Serial.println("Characteristic defined! Now you can read it in your phone!");


  ledcSetup(0, 8000, 8);
  ledcSetup(1, 8000, 3);
  ledcSetup(2, 8000, 8);
  ledcSetup(3, 8000, 8);
  ledcAttachPin(32, 0);
  ledcAttachPin(33, 1);
  ledcAttachPin(25, 2);
  ledcAttachPin(26, 3);

}

uint16_t  rpm   = 0; //0～1023(10bit) 0～65536
uint8_t   shift = 0; //0～7(3bit) 0～255
uint8_t   temp  = 0; //-10～245(8bit)
uint8_t   batt  = 0; //0～255(8bit)

void loop() {
  if (deviceConnected) {


    uint8_t buff[5] = {0};
    char str[50];
    buff[0] = (uint8_t)(rpm >> 8);
    buff[1] = (uint8_t)(rpm >> 0);
    buff[2] = shift;
    buff[3] = temp;
    buff[4] = batt;

    sprintf(str, "%04d ,%d ,%3d ,%3d", rpm,shift,temp,batt);
    Serial.println(str);
    pCharacteristic->setValue(buff, 5);
    pCharacteristic->notify();
  }

  ledcWrite(0, rpm);
  ledcWrite(1, shift);
  ledcWrite(2, temp);
  ledcWrite(3, batt);

  rpm = (rpm + 1) % 800;
  shift = (shift + 1) % 5;
  temp = (temp + 1) % 100;
  batt = (batt + 1) % 155;
  delay(100);
}
