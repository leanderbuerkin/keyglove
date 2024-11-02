#include <Arduino.h>
#include <Wire.h>

#define ADDRESS_ACCELEROMETER_GYROSCOPE_1 0x68
#define POWER_MANAGMENT_REGISTER_ADDRESS 0x6B
#define WAKE_UP_MESSAGE_TO_POWER_MANAGMENT_REGISTER 0
#define ADDRESS_FIRST_DATA_REGISTER 0x3B
#define NUMBER_OF_DATA_REGISTERS 14
#define MINIMAL_NUMBER_OF_DATA_REGISTERS 5

int acceleration_x, acceleration_y, acceleration_z;
int rotation_x, rotation_y, rotation_z, temperature;

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
  Wire.beginTransmission(sensor_address);
  Wire.write(ADDRESS_FIRST_DATA_REGISTER);
  Wire.endTransmission(false);
  Wire.requestFrom(sensor_address, MINIMAL_NUMBER_OF_DATA_REGISTERS, true);
  acceleration_x = Wire.read();
  Wire.read();
  acceleration_y = Wire.read();
  Wire.read();
  acceleration_z = Wire.read();
}

void setup()
{
  Serial.begin(921600);
  setup_accelerometer_gyroscope(ADDRESS_ACCELEROMETER_GYROSCOPE_1);
}

void loop()
{
  read_accelerometer(ADDRESS_ACCELEROMETER_GYROSCOPE_1);
  //Serial.print(acceleration_x);
  //Serial.print(acceleration_y);
  Serial.println(acceleration_z);
}
