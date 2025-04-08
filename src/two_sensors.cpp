#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define MPU_ADDRESS 0x68
#define MPU_ADDRESS_2 0x69
#define POWER_MANAGMENT_REGISTER_ADDRESS 0x6B
#define WAKE_UP_MESSAGE_TO_POWER_MANAGMENT 0
#define X_ACCELERATION_REGISTER_ADDRESS 0x3B
#define Z_ACCELERATION_REGISTER_ADDRESS 0x3F
#define NUMBER_OF_NEEDED_REGISTERS 1
#define I2C_CLOCK_FREQUENCY 400000
#define MPU_1_SDA_PIN 18
#define MPU_1_SCL_PIN 19
#define MPU_2_SDA_PIN 21
#define MPU_2_SCL_PIN 22

TwoWire I2C_bus_1 = TwoWire(0);
TwoWire I2C_bus_2 = TwoWire(1);

Adafruit_MPU6050 mpu1;
Adafruit_MPU6050 mpu2;
Adafruit_MPU6050 mpu3;
Adafruit_MPU6050 mpu4;

void setup(void)
{
    Serial.begin(921600);
    I2C_bus_1.begin(MPU_1_SDA_PIN, MPU_1_SCL_PIN, I2C_CLOCK_FREQUENCY);
    I2C_bus_2.begin(MPU_2_SDA_PIN, MPU_2_SCL_PIN, I2C_CLOCK_FREQUENCY);

    mpu1.begin(MPU_ADDRESS, &I2C_bus_1);
    mpu2.begin(MPU_ADDRESS, &I2C_bus_2);
    mpu3.begin(MPU_ADDRESS_2, &I2C_bus_1);
    mpu4.begin(MPU_ADDRESS_2, &I2C_bus_2);

    mpu1.setFilterBandwidth(MPU6050_BAND_5_HZ);
    mpu2.setFilterBandwidth(MPU6050_BAND_5_HZ);
    mpu3.setFilterBandwidth(MPU6050_BAND_5_HZ);
    mpu4.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

void request_data_from_sensor(TwoWire &I2C_bus, bool second_sensor)
{
    if (second_sensor)
    {
        I2C_bus.beginTransmission(MPU_ADDRESS_2);
        I2C_bus.write(X_ACCELERATION_REGISTER_ADDRESS);
        I2C_bus.endTransmission(false);
        I2C_bus.requestFrom(MPU_ADDRESS_2, NUMBER_OF_NEEDED_REGISTERS);
    }
    else {
        I2C_bus.beginTransmission(MPU_ADDRESS);
        I2C_bus.write(X_ACCELERATION_REGISTER_ADDRESS);
        I2C_bus.endTransmission(false);
        I2C_bus.requestFrom(MPU_ADDRESS, NUMBER_OF_NEEDED_REGISTERS);
    }
    
}

void send_via_serial_to_teleplot(void)
{
    request_data_from_sensor(I2C_bus_1, false);
    Serial.print(">Sensor1:");
    // use the following for more continous values, without it is faster
    Serial.println(static_cast<int8_t>(I2C_bus_1.read()));
    // Serial.println(static_cast<int8_t>(acceleration_sensor_1));

    request_data_from_sensor(I2C_bus_2, false);
    Serial.print(">Sensor2:");
    // use the following for more continous values, without it is faster
    Serial.println(static_cast<int8_t>(I2C_bus_2.read()));
    // Serial.println(static_cast<int8_t>(acceleration_sensor_2));

    request_data_from_sensor(I2C_bus_1, true);
    Serial.print(">Sensor3:");
    // use the following for more continous values, without it is faster
    Serial.println(static_cast<int8_t>(I2C_bus_1.read()));
    // Serial.println(static_cast<int8_t>(acceleration_sensor_1));

    request_data_from_sensor(I2C_bus_2, true);
    Serial.print(">Sensor4:");
    // use the following for more continous values, without it is faster
    Serial.println(static_cast<int8_t>(I2C_bus_2.read()));
    // Serial.println(static_cast<int8_t>(acceleration_sensor_2));
}

void loop(void)
{
    send_via_serial_to_teleplot();
}
