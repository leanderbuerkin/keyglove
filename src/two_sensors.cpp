#include <Arduino.h>
#include <Wire.h>

///// ToDo: Reads Z data register now, document!!

#define MPU_ADDRESS 0x68
#define POWER_MANAGMENT_REGISTER_ADDRESS 0x6B
#define WAKE_UP_MESSAGE_TO_POWER_MANAGMENT 0
#define FIRST_DATA_REGISTER_ADDRESS 0x3B
#define Z_ACCELERATION_DATA_REGISTER_ADDRESS 0x3F
#define NUMBER_OF_NEEDED_REGISTERS 1
#define I2C_CLOCK_FREQUENCY 400000
#define FIRST_MPU_SDA_PIN 18
#define FIRST_MPU_SCL_PIN 19
#define SECOND_MPU_SDA_PIN 21
#define SECOND_MPU_SCL_PIN 22
#define SERIAL_TRANSMISSION_START_MESSAGE_SENSOR_1 "ONE"
#define SERIAL_TRANSMISSION_START_MESSAGE_SENSOR_2 "TWO"

TwoWire I2C_bus_1 = TwoWire(0);
TwoWire I2C_bus_2 = TwoWire(1);

void setup_mpu(TwoWire &I2C_bus)
{
    I2C_bus.beginTransmission(MPU_ADDRESS);
    I2C_bus.write(POWER_MANAGMENT_REGISTER_ADDRESS);
    I2C_bus.write(WAKE_UP_MESSAGE_TO_POWER_MANAGMENT);
    I2C_bus.endTransmission(true);
}

void setup()
{
    Serial.begin(921600);
    I2C_bus_1.begin(FIRST_MPU_SDA_PIN, FIRST_MPU_SCL_PIN, I2C_CLOCK_FREQUENCY);
    I2C_bus_2.begin(SECOND_MPU_SDA_PIN, SECOND_MPU_SCL_PIN, I2C_CLOCK_FREQUENCY);
    setup_mpu(I2C_bus_1);
    setup_mpu(I2C_bus_2);
}

void request_one_acceleration_high_byte(TwoWire &I2C_bus)
{
    I2C_bus.beginTransmission(MPU_ADDRESS);
    I2C_bus.write(Z_ACCELERATION_DATA_REGISTER_ADDRESS);
    I2C_bus.endTransmission(false);
    I2C_bus.requestFrom(MPU_ADDRESS, NUMBER_OF_NEEDED_REGISTERS);
}

void serial_to_teleplot()
{
    Serial.print(">Sensor1:");
    Serial.println(static_cast<int8_t>(I2C_bus_1.read()));
    Serial.print(">Sensor2:");
    Serial.println(static_cast<int8_t>(I2C_bus_2.read()));
}

void serial_to_python()
{
    Serial.print(static_cast<int8_t>(I2C_bus_1.read()));
    Serial.print(" ");
    Serial.println(static_cast<int8_t>(I2C_bus_2.read()));
}

void loop()
{
    request_one_acceleration_high_byte(I2C_bus_1);
    request_one_acceleration_high_byte(I2C_bus_2);
    serial_to_python();
}
