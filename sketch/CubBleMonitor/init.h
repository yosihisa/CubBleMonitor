void OTA_setup() {
  //WiFi,OTA 設定
  Serial.println("Init WiFi");
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting WiFi");
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    Serial.println("WiFi Connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("init OTA");

  ArduinoOTA.setHostname("CubBleMonitor");
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  Serial.println("Begin OTA");
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}


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

void GNSS_setup() {
  Serial.println("Init GNSS.");
  Serial_GNSS.begin(9600);
  delay(200);
  for (int i = 0; i < 3; i++) {
    Serial_GNSS.print("$PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    delay(100);
  }

  for (int i = 0; i < 3; i++) {
    Serial_GNSS.print("$PMTK300,100,0,0,0,0*2C\r\n");
    delay(100);
  }

  for (int i = 0; i < 3; i++) {
    Serial_GNSS.print("$PMTK220,100*2F\r\n");
    delay(100);
  }
  Serial_GNSS.setTimeout(500);
}

void PIN_setup() {
  Serial.println("Init PIN.");

  pinMode(RPM_PIN, INPUT);

  pinMode(SHIFT_1_PIN, INPUT);
  pinMode(SHIFT_2_PIN, INPUT);
  pinMode(SHIFT_3_PIN, INPUT);
  pinMode(SHIFT_4_PIN, INPUT);

  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);

  Serial.println("Init LED PWM.");
  ledcSetup(LED_R_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(LED_G_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(LED_B_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(LED_R_PIN, LED_R_CHANNEL);
  ledcAttachPin(LED_G_PIN, LED_G_CHANNEL);
  ledcAttachPin(LED_B_PIN, LED_B_CHANNEL);
}
