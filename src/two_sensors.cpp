#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

TwoWire first_I2C_bus = TwoWire(0);
TwoWire second_I2C_bus = TwoWire(1);

int second_mpu_SDA_pin = 18;
int second_mpu_SCL_pin = 19;
int first_mpu_SDA_pin = 21;
int first_mpu_SCL_pin = 22;

int I2C_clock_frequency = 400000;

Adafruit_MPU6050 mpu1;
Adafruit_MPU6050 mpu2;

void initialize_mpu(TwoWire I2C_bus) {
    if (!mpu1.begin(0x68, &I2C_bus))
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1) delay(10);
    }
    Serial.println("MPU6050 Found!");
}

void setup() {
    Serial.begin(921600);
    while (!Serial) delay(10);

    first_I2C_bus.begin(first_mpu_SDA_pin, first_mpu_SCL_pin, I2C_clock_frequency);

    second_I2C_bus.begin(second_mpu_SDA_pin, second_mpu_SCL_pin, I2C_clock_frequency);

    Serial.println("Initializing MPU6050 sensors...");

    // Initialize the first MPU6050
    if (!mpu1.begin(0x68, &first_I2C_bus))
    {
        Serial.println("Failed to find first MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("First MPU6050 Found!");

    // Initialize the second MPU6050
    if (!mpu2.begin(0x68, &second_I2C_bus))
    {
        Serial.println("Failed to find second MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("Second MPU6050 Found!");

    mpu1.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu1.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ);

    mpu2.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu2.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ);

    delay(100);
}

void loop()
{
    // Read data from the first MPU6050
    sensors_event_t a1, g1, temp1;
    mpu1.getEvent(&a1, &g1, &temp1);

    // Read data from the second MPU6050
    sensors_event_t a2, g2, temp2;
    mpu2.getEvent(&a2, &g2, &temp2);

    Serial.print(">acceleration_x1:");
    Serial.println(a1.acceleration.x);
    Serial.print(">acceleration_y1:");
    Serial.println(a1.acceleration.y);
    Serial.print(">acceleration_z1:");
    Serial.println(a1.acceleration.z);

    Serial.print(">acceleration_x2:");
    Serial.println(a2.acceleration.x);
    Serial.print(">acceleration_y2:");
    Serial.println(a2.acceleration.y);
    Serial.print(">acceleration_z2:");
    Serial.println(a2.acceleration.z);
}
