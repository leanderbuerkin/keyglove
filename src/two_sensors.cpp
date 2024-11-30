#include <Arduino.h>
#include <Wire.h>
#include <chrono>

#define MPU_ADDRESS 0x68
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

void setup_mpu(TwoWire &I2C_bus)
{
    I2C_bus.beginTransmission(MPU_ADDRESS);
    I2C_bus.write(POWER_MANAGMENT_REGISTER_ADDRESS);
    I2C_bus.write(WAKE_UP_MESSAGE_TO_POWER_MANAGMENT);
    I2C_bus.endTransmission(true);
}

void setup(void)
{
    Serial.begin(921600);
    I2C_bus_1.begin(MPU_1_SDA_PIN, MPU_1_SCL_PIN, I2C_CLOCK_FREQUENCY);
    I2C_bus_2.begin(MPU_2_SDA_PIN, MPU_2_SCL_PIN, I2C_CLOCK_FREQUENCY);
    setup_mpu(I2C_bus_1);
    setup_mpu(I2C_bus_2);
}

void get_data_from_sensor(TwoWire &I2C_bus)
{
    I2C_bus.beginTransmission(MPU_ADDRESS);
    I2C_bus.write(X_ACCELERATION_REGISTER_ADDRESS);
    I2C_bus.endTransmission(false);
    I2C_bus.requestFrom(MPU_ADDRESS, NUMBER_OF_NEEDED_REGISTERS);
}

void send_via_serial_to_teleplot(void)
{
    Serial.print(">Sensor1:");
    Serial.println(static_cast<int8_t>(I2C_bus_1.read()));
    Serial.print(">Sensor2:");
    Serial.println(static_cast<int8_t>(I2C_bus_2.read()));
}

void send_via_serial_to_python(void)
{
    Serial.print(static_cast<int8_t>(I2C_bus_1.read()));
    Serial.print(" ");
    Serial.println(static_cast<int8_t>(I2C_bus_2.read()));
}


double measureExecutionsPerSecond(double durationSeconds)
{
    using namespace std::chrono;

    auto start = high_resolution_clock::now();
    auto end = start + duration_cast<high_resolution_clock::duration>(duration<double>(durationSeconds));

    int executionCount = 0;
    while (high_resolution_clock::now() < end)
    {
        {
            get_data_from_sensor(I2C_bus_1);
            get_data_from_sensor(I2C_bus_2);
            executionCount++;
        }

        auto actualDuration = duration_cast<duration<double>>(high_resolution_clock::now() - start).count();
        Serial.println(executionCount / actualDuration);
    }
}

void loop(void)
{
    measureExecutionsPerSecond(10);

    //get_data_from_sensor(I2C_bus_1);
    //get_data_from_sensor(I2C_bus_2);
    // send_via_serial_to_python();
}
