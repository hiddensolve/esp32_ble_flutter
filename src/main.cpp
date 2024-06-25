#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

BLEServer *pServer;
BLEService *pLightControlService;
BLECharacteristic *pBulb1Characteristic;

const char *SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char *CHARACTERISTIC_BULB1_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

bool bulb1State = false;  // Initial state of bulb1

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Device connected");
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Device disconnected");
    }
};

class Bulb1CharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() == 1) {
            if (value[0] == 0x01) {
                Serial.println("Bulb1 turned on");
                digitalWrite(LED_BUILTIN, HIGH);  // Replace with your actual GPIO pin for controlling the bulb
                bulb1State = true;
            } else if (value[0] == 0x00) {
                Serial.println("Bulb1 turned off");
                digitalWrite(LED_BUILTIN, LOW);  // Replace with your actual GPIO pin for controlling the bulb
                bulb1State = false;
            }
        }
    }
};

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Initialize BLE
    BLEDevice::init("ESP32 Light Control");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create service
    pLightControlService = pServer->createService(SERVICE_UUID);

    // Create characteristic
    pBulb1Characteristic = pLightControlService->createCharacteristic(
        CHARACTERISTIC_BULB1_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    // Set characteristic callbacks
    pBulb1Characteristic->setCallbacks(new Bulb1CharacteristicCallbacks());

    // Start service
    pLightControlService->start();

    // Advertising
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(BLEUUID(SERVICE_UUID));
    


    // Set appearance (optional)
    pAdvertising->setAppearance(384); // Generic Light Bulb
    
    // Start advertising
    pAdvertising->start();

    Serial.println("BLE server started. Waiting for connections...");
}

void loop() {
    // Nothing to do here for a BLE server
    delay(2000);
}
