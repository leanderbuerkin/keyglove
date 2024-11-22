#include <Arduino.h>
#include <Wire.h>

#define MPU_ADDRESS 0x68
#define POWER_MANAGMENT_REGISTER_ADDRESS 0x6B
#define WAKE_UP_MESSAGE_TO_POWER_MANAGMENT 0
#define FIRST_DATA_REGISTER_ADDRESS 0x3B
#define NUMBER_OF_NEEDED_REGISTERS 1
#define I2C_CLOCK_FREQUENCY 400000
#define FIRST_MPU_SDA_PIN 18
#define FIRST_MPU_SCL_PIN 19
#define SECOND_MPU_SDA_PIN 21
#define SECOND_MPU_SCL_PIN 22
#define SERIAL_TRANSMISSION_START_MESSAGE "go"

TwoWire first_I2C_bus = TwoWire(0);
TwoWire second_I2C_bus = TwoWire(1);

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
    first_I2C_bus.begin(FIRST_MPU_SDA_PIN, FIRST_MPU_SCL_PIN, I2C_CLOCK_FREQUENCY);
    second_I2C_bus.begin(SECOND_MPU_SDA_PIN, SECOND_MPU_SCL_PIN, I2C_CLOCK_FREQUENCY);
    setup_mpu(first_I2C_bus);
    setup_mpu(second_I2C_bus);
}

void request_acceleration_x_high_byte(TwoWire &I2C_bus)
{
    I2C_bus.beginTransmission(MPU_ADDRESS);
    I2C_bus.write(FIRST_DATA_REGISTER_ADDRESS);
    I2C_bus.endTransmission(false);
    I2C_bus.requestFrom(MPU_ADDRESS, NUMBER_OF_NEEDED_REGISTERS);
}

void loop()
{
    Serial.println(SERIAL_TRANSMISSION_START_MESSAGE);
    request_acceleration_x_high_byte(first_I2C_bus);
    Serial.println(first_I2C_bus.read());
    request_acceleration_x_high_byte(second_I2C_bus);
    Serial.println(second_I2C_bus.read());
}
