#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

//BLEのUUID
#define SERVICE_UUID "90af54ef-d367-41ec-ac04-b3f9f42cdcb2"
#define CHARACTERISTIC_UUID "a018d2c9-5838-4196-bd97-d00547de2c57"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

uint16_t  rpm   = 0;    //0～1023(10bit) 0～65536
uint8_t   shift = 0; //0～7(3bit) 0～255
uint8_t   temp  = 0; //-10～245(8bit)
uint8_t   batt  = 0; //0～255(8bit)
uint16_t  speed = 0; //0～255(8bit)
uint8_t   mode  = 0; //N A D E


class BLE_ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Disonnected");
    }
};

void BLE_setup() {
  // Create the BLE Device
  BLEDevice::init("Cub Avio");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLE_ServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setValue("Hello World");

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start the service
  pServer->getAdvertising()->start();
  BLEDevice::startAdvertising();
}


void setup() {
  Serial.begin(115200);
  Serial.println("Cub Avio");

  //BLE開始
  BLE_setup();
}

void loop() {

  rpm = (rpm + 10) % 9500;
  shift = shift + 1;
  batt = (batt + 1) % 160;
  speed = (speed + 2) % 800;
  temp = temperatureRead() + 20;

  //BLE送信
  if (deviceConnected) {
    uint8_t BLE_tx_buff[8] = {0};

    BLE_tx_buff[0] = (uint8_t)(rpm >> 8);
    BLE_tx_buff[1] = (uint8_t)(rpm >> 0);
    BLE_tx_buff[2] = shift;
    BLE_tx_buff[3] = temp;
    BLE_tx_buff[4] = batt;
    BLE_tx_buff[5] = (uint8_t)(speed >> 8);
    BLE_tx_buff[6] = (uint8_t)(speed >> 0);
    BLE_tx_buff[7] = mode;

    pCharacteristic->setValue(BLE_tx_buff, 8);
    pCharacteristic->notify();

  }
  char str[50];
  sprintf(str, "%04d ,%d ,%3d ,%3d ,%3d %c ", rpm, shift, temp, batt, speed, mode);
  Serial.println(str);
  delay(100);
}

