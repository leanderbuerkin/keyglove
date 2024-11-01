#include <Arduino.h>
#include <Wire.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define ADDRESS_ACCELEROMETER_GYROSCOPE_1 0x68
#define NUMBER_OF_DATA_REGISTERS 14
#define MINIMAL_NUMBER_OF_DATA_REGISTERS 5
#define POWER_MANAGMENT_REGISTER_ADDRESS 0x6B
#define WAKE_UP_MESSAGE_TO_POWER_MANAGMENT_REGISTER 0

#define ACCELEROMETER_X_OUT_HIGH_BYTE 0x3B
#define ACCELEROMETER_Y_OUT_HIGH_BYTE 0x3D
#define ACCELEROMETER_Z_OUT_HIGH_BYTE 0x3F

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

int acceleration_x;
int acceleration_y;
int acceleration_z;
int rotation_x;
int rotation_y;
int rotation_z;
int temperature;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool device_connected = false;

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

void read_accelerometer(int sensor_address)
{
  // 1114 meassurements per second with 14 registers (low and high bytes saved)
  // 1866 meassurements per second with 6 registers (low and high bytes saved)
  // 2030 meassurements per second with 5 registers (high bytes saved)
  // 1302 meassurements per second with 3 registers (high bytes saved),
  //      each of them individually called
  // 400 kHz clock speed:
  // 1126 meassurements per second with 14 registers (low and high bytes saved)
  // 2120 meassurements per second with 5 registers (high bytes saved)
  Wire.beginTransmission(sensor_address);
  Wire.write(ACCELEROMETER_X_OUT_HIGH_BYTE);
  Wire.endTransmission(false);
  Wire.requestFrom(sensor_address, MINIMAL_NUMBER_OF_DATA_REGISTERS, true);
  acceleration_x = Wire.read();
  Wire.read();
  acceleration_y = Wire.read();
  Wire.read();
  acceleration_z = Wire.read();
}

void read_accelerometer_gyroscope(int sensor_address) {
  Wire.beginTransmission(sensor_address);
  Wire.write(ACCELEROMETER_X_OUT_HIGH_BYTE);
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
    device_connected = true;
  }

  void onDisconnect(BLEServer *pServer)
  {
    device_connected = false;
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
  setup_accelerometer_gyroscope(ADDRESS_ACCELEROMETER_GYROSCOPE_1);
}

void loop()
{
  if (device_connected)
  {
    read_accelerometer(ADDRESS_ACCELEROMETER_GYROSCOPE_1);

    pCharacteristic->setValue(dataPointsPerSecond);
    pCharacteristic->notify();
    pCharacteristic->setValue(acceleration_x);
    pCharacteristic->notify();
    pCharacteristic->setValue(acceleration_y);
    pCharacteristic->notify();
    pCharacteristic->setValue(acceleration_z);
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
