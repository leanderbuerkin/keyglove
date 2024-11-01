#include <Arduino.h>
#include <Wire.h>

#define ADDRESS_acceleration_rotation_1 0x68
#define FIRST_DATA_REGISTER 0x3B
#define NUMBER_OF_DATA_REGISTERS 14
#define POWER_MANAGMENT_REGISTER_ADDRESS 0x6B
#define WAKE_UP_MESSAGE_TO_POWER_MANAGMENT_REGISTER 0

int16_t acceleration_x;
int16_t acceleration_y;
int16_t acceleration_z;
int16_t rotation_x;
int16_t rotation_y;
int16_t rotation_z;
int16_t temperature;

void setup_mpu(int sensor_address)
{
  Wire.begin();
  Wire.beginTransmission(sensor_address);
  Wire.write(POWER_MANAGMENT_REGISTER_ADDRESS);
  Wire.write(WAKE_UP_MESSAGE_TO_POWER_MANAGMENT_REGISTER);
  Wire.endTransmission(true);
}

void read_mpu(int sensor_address)
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

void setup()
{
  Serial.begin(115200); // Increase baud rate
  setup_mpu(ADDRESS_acceleration_rotation_1);
}

void loop()
{
  read_mpu(ADDRESS_acceleration_rotation_1);
  Serial.print(">acceleration_x:");
  Serial.println(acceleration_x);
  Serial.print(">acceleration_y:");
  Serial.println(acceleration_y);
  Serial.print(">acceleration_z:");
  Serial.println(acceleration_z);
  Serial.print(">rotation_x:");
  Serial.println(rotation_x);
  Serial.print(">rotation_y:");
  Serial.println(rotation_y);
  Serial.print(">rotation_z:");
  Serial.println(rotation_z);
  Serial.print(">temperature:");
  Serial.println(temperature);
}
