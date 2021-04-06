#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

//OTAアップデート用のWiFiアクセスポイントの設定
const char *ssid = "ESP-UPDATE";
const char *password = "ESP32ESP32";

//BLEのUUID
#define SERVICE_UUID "90af54ef-d367-41ec-ac04-b3f9f42cdcb2"
#define CHARACTERISTIC_UUID "a018d2c9-5838-4196-bd97-d00547de2c57"

//点火パルス
#define RPM_PIN 18
#define SMOOTHING 3

//LED
#define LED_R_PIN 25
#define LED_G_PIN 26
#define LED_B_PIN 27

#define LEDC_TIMER_BIT 8
#define LEDC_BASE_FREQ 12800
#define LED_R_CHANNEL 0
#define LED_G_CHANNEL 1
#define LED_B_CHANNEL 2

//シフトインジケータ
#define SHIFT_1_PIN 19
#define SHIFT_2_PIN 21
#define SHIFT_3_PIN 22
#define SHIFT_4_PIN 23

//バッテリー電圧
#define BATT_PIN 33

//基板温度
#define TEMP_PIN 32

//GNSS
HardwareSerial Serial_GNSS(2); // UART2 (RX=16, TX=17)

//BLE
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

//Timer
#define COUNTER_PERIOD 30* 1000 * 100
#define COUNTER_DIVIDE 800
uint64_t timeCounter;
uint64_t ignition_cycle[SMOOTHING];
uint8_t ignition_num;

uint16_t  rpm   = 0;    //0～1023(10bit) 0～65536
uint8_t   shift = 0; //0～7(3bit) 0～255
uint8_t   temp  = 0; //-10～245(8bit)
uint8_t   batt  = 0; //0～255(8bit)
uint16_t  speed = 0; //0～255(8bit)
uint8_t   mode  = 0; //N A D E

bool ota_mode = false;
int cnt;

#include "init.h"
#include "function.h"


void IRAM_ATTR rpm_pulse() {
  uint64_t now_time = esp_timer_get_time();
  ignition_cycle[ignition_num] = now_time - timeCounter;
  ignition_num = (ignition_num + 1) % SMOOTHING;
  timeCounter = now_time;
  return;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Cub Avio");

  PIN_setup();
  led_rgb(255, 255, 255);
  GNSS_setup();

  //3速で起動したらOTAを有効にする
  if (digitalRead(SHIFT_3_PIN) == LOW){
    led_rgb(255, 50, 50);
    OTA_setup();
    ota_mode = true;
    for (int i = 0; i<5; i++) {
      led_rgb(0, 255, 0);
      delay(200);
      led_rgb(0, 0, 0);
      delay(200);
    }
  }
  
  //BLE開始
  BLE_setup();

  //点火パルスの割り込み開始
  ignition_num = 0;
  for(int i=0;i< SMOOTHING;i++){
    ignition_cycle[i] = 0;
  }

  if (ota_mode != true){
    attachInterrupt(RPM_PIN, rpm_pulse, RISING);
  }
  led_rgb(0, 0, 0);
}

void loop() {

  if ( ota_mode == true ) ArduinoOTA.handle();

  uint64_t sum_cycle = 0;
  for (int i = 0; i < SMOOTHING; i++) {
    sum_cycle += ignition_cycle[i];
  }
  if (sum_cycle!=0){
    rpm = (uint32_t)1000 * 1000 * 60 * 2 * SMOOTHING / sum_cycle;
  }else{
    rpm = 0;
  }
  shift = read_shift();
  batt = (uint32_t) analogRead(BATT_PIN) * 33 * (1000) / 165 / 4096;
  temp = (float)-0.0259 * analogRead(TEMP_PIN) + 78.263 + 20;

  
  //GNSS取得
  String cmds[15] = {"\0"};
  String data = Serial_GNSS.readStringUntil('\n'); //1行読み込み
  int index = split(data, ',', cmds);              // , で分割
  if(index >=10){
    speed = cmds[7].toFloat() * 10;                //速度を取り出し
    mode  = cmds[9].charAt(0);                     //取得状況を取り出し
  }

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

  //シフトインジケータ表示
  indicate_LED();

  char str[50];
  sprintf(str, "%04d ,%d ,%3d ,%3d ,%3d [%d] ,%ld", rpm, shift, temp - 20, batt, speed, mode, ignition_cycle[0]);
  Serial.println(str);
}

