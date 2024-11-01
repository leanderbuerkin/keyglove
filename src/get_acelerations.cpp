#include <Arduino.h>
#include <Wire.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define ADDRESS_acceleration_rotation_1 0x68
#define FIRST_DATA_REGISTER 0x3B
#define NUMBER_OF_DATA_REGISTERS 14
#define POWER_MANAGMENT_REGISTER_ADDRESS 0x6B
#define WAKE_UP_MESSAGE_TO_POWER_MANAGMENT_REGISTER 0

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

int16_t acceleration_x;
int16_t acceleration_y;
int16_t acceleration_z;
int16_t rotation_x;
int16_t rotation_y;
int16_t rotation_z;
int16_t temperature;

unsigned long lastSecond = 0;
unsigned int dataPointCount = 0;
unsigned int dataPointsPerSecond = 0;

void setup_accelerometer_gyroscope(int sensor_address)
{
  Wire.begin();
  Wire.beginTransmission(sensor_address);
  Wire.write(POWER_MANAGMENT_REGISTER_ADDRESS);
  Wire.write(WAKE_UP_MESSAGE_TO_POWER_MANAGMENT_REGISTER);
  Wire.endTransmission(true);
}

void read_accelerometer_gyroscope(int sensor_address)
{
  Wire.beginTransmission(sensor_address);
  Wire.write(FIRST_DATA_REGISTER);
  Wire.endTransmission(false);
  Wire.requestFrom(sensor_address, NUMBER_OF_DATA_REGISTERS, true);
  acceleration_x = Wire.read() << 8 | Wire.read();
  acceleration_y = Wire.read() << 8 | Wire.read();
  acceleration_z = Wire.read() << 8 | Wire.read();
  temperature = Wire.read() << 8 | Wire.read();
  rotation_x = Wire.read() << 8 | Wire.read();
  rotation_y = Wire.read() << 8 | Wire.read();
  rotation_z = Wire.read() << 8 | Wire.read();
}

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

void setup_bluetooth()
{
  Serial.begin(115200);
  BLEDevice::init("ESP32");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection to notify...");
}

void setup()
{
  setup_bluetooth();
  setup_accelerometer_gyroscope(ADDRESS_acceleration_rotation_1);
}

void loop()
{
  if (deviceConnected)
  {
    read_accelerometer_gyroscope(ADDRESS_acceleration_rotation_1);

    pCharacteristic->setValue(dataPointsPerSecond);
    pCharacteristic->notify();
    dataPointCount++;
  }
  // Check if a second has passed
  if (millis() - lastSecond >= 1000)
  {
    dataPointsPerSecond = dataPointCount;
    dataPointCount = 0;
    lastSecond = millis();
  }
  dataPointCount++;
}
