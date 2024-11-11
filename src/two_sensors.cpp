#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Create two TwoWire instances for the two I2C buses
TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);

// Create two MPU6050 instances
Adafruit_MPU6050 mpu1;
Adafruit_MPU6050 mpu2;

void setup()
{
    Serial.begin(921600);
    while (!Serial)
        delay(10); // Wait for Serial Monitor to open

    // Initialize the first I2C bus (default pins: SDA=21, SCL=22)
    I2Cone.begin(21, 22, 400000);

    // Initialize the second I2C bus (example pins: SDA=18, SCL=19)
    I2Ctwo.begin(18, 19, 400000);

    Serial.println("Initializing MPU6050 sensors...");

    // Initialize the first MPU6050
    if (!mpu1.begin(0x68, &I2Cone))
    {
        Serial.println("Failed to find first MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("First MPU6050 Found!");

    // Initialize the second MPU6050
    if (!mpu2.begin(0x68, &I2Ctwo))
    {
        Serial.println("Failed to find second MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("Second MPU6050 Found!");

    // Set up the sensors
    mpu1.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu1.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ);

    mpu2.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu2.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ);

    delay(100);
}

void printSensorData(sensors_event_t a, sensors_event_t g, sensors_event_t temp)
{
    Serial.print(">acceleration_x:");
    Serial.println(a.acceleration.x);
    Serial.print(">acceleration_y:");
    Serial.println(a.acceleration.y);
    Serial.print(">acceleration_z:");
    Serial.println(a.acceleration.z);

}

void loop()
{
    // Read data from the first MPU6050
    sensors_event_t a1, g1, temp1;
    mpu1.getEvent(&a1, &g1, &temp1);

    // Read data from the second MPU6050
    sensors_event_t a2, g2, temp2;
    mpu2.getEvent(&a2, &g2, &temp2);

    // Print the data from both sensors
    Serial.print(">acceleration_x1:");
    Serial.println(a1.acceleration.x);
    Serial.print(">acceleration_y1:");
    Serial.println(a1.acceleration.y);
    Serial.print(">acceleration_z1:");
    Serial.println(a1.acceleration.z);
    // Print the data from both sensors
    Serial.print(">acceleration_x2:");
    Serial.println(a2.acceleration.x);
    Serial.print(">acceleration_y2:");
    Serial.println(a2.acceleration.y);
    Serial.print(">acceleration_z2:");
    Serial.println(a2.acceleration.z);
}
