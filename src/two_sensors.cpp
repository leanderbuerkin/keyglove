#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>

TwoWire first_I2C_bus = TwoWire(0);
TwoWire second_I2C_bus = TwoWire(1);

int second_mpu_SDA_pin = 18;
int second_mpu_SCL_pin = 19;
int first_mpu_SDA_pin = 21;
int first_mpu_SCL_pin = 22;

int I2C_clock_frequency = 400000;

Adafruit_MPU6050 first_accelerometer;
Adafruit_MPU6050 second_accelerometer;

void initialize_mpu(TwoWire &I2C_bus, Adafruit_MPU6050 &accelerometer)
{
    Serial.println("Initializing MPU6050 sensor");
    if (!accelerometer.begin(0x68, &I2C_bus))
    {
        Serial.println("MPU6050 chip not found");
        while (1);
    }
    Serial.println("MPU6050 found");

    accelerometer.setAccelerometerRange(MPU6050_RANGE_8_G);
    accelerometer.setGyroRange(MPU6050_RANGE_500_DEG);
    accelerometer.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void setup() {
    Serial.begin(921600);
    while (!Serial) delay(10);

    first_I2C_bus.begin(first_mpu_SDA_pin, first_mpu_SCL_pin, I2C_clock_frequency);
    second_I2C_bus.begin(second_mpu_SDA_pin, second_mpu_SCL_pin, I2C_clock_frequency);

    initialize_mpu(first_I2C_bus, first_accelerometer);
    initialize_mpu(second_I2C_bus, second_accelerometer);

    delay(100);
}

void loop()
{
    sensors_event_t a1, g1, temp1;
    first_accelerometer.getEvent(&a1, &g1, &temp1);

    sensors_event_t a2, g2, temp2;
    second_accelerometer.getEvent(&a2, &g2, &temp2);

    Serial.print(">acceleration_x1:");
    Serial.println(a1.acceleration.x);

    Serial.print(">acceleration_x2:");
    Serial.println(a2.acceleration.x);
}
