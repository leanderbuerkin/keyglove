#include <Arduino.h>
#include <Wire.h>

#define MPU_1 0x68
#define PIN 33

int16_t accX, accY, accZ, gyrX, gyrY, gyrZ, tVal;
int temperature = 0.0;
int calAccX = 0;
int calAccY = 0;
int calAccZ = 0;
int calGyrX = 0;
int calGyrY = 0;
int calGyrZ = 0;

void setup_mpu(int mpu)
{
  Wire.begin();
  Wire.beginTransmission(mpu); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B);            // PWR_MGMT_1 register
  Wire.write(0);               // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}
void read_mpu(int mpu)
{
  // set starting register
  Wire.beginTransmission(mpu);
  Wire.write(0x3B);                   // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false);        // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(mpu, 7 * 2, true);    // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accX = (Wire.read() << 8 | Wire.read());        // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accY = (Wire.read() << 8 | Wire.read());        // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accZ = (Wire.read() << 8 | Wire.read());        // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  tVal = Wire.read() << 8 | Wire.read();          // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyrX = (Wire.read() << 8 | Wire.read()) / 65.5; // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyrY = (Wire.read() << 8 | Wire.read()) / 65.5; // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyrZ = (Wire.read() << 8 | Wire.read()) / 65.5; // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
}
void calibration(int mpu, int precission)
{
  for (int i = 0; i < precission; i++)
  {
    read_mpu(mpu);
    calAccX += accX;
    calAccY += accY;
    calAccZ += accZ;
    calGyrX += gyrX;
    calGyrY += gyrY;
    calGyrZ += gyrZ;
  }
  calAccX /= precission;
  calAccY /= precission;
  calAccZ /= precission;
  calGyrX /= precission;
  calGyrY /= precission;
  calGyrZ /= precission;
}
void smooth(int mpu, int precission)
{
  int AccX = 0;
  int AccY = 0;
  int AccZ = 0;
  int GyrX = 0;
  int GyrY = 0;
  int GyrZ = 0;
  for (int i = 0; i < precission; i++)
  {
    read_mpu(mpu);
    AccX += accX;
    AccY += accY;
    AccZ += accZ;
    GyrX += gyrX;
    GyrY += gyrY;
    GyrZ += gyrZ;
  }
  accX = (AccX / precission) - calAccX;
  accY = (AccY / precission) - calAccY;
  accZ = (AccZ / precission) - calAccZ;
  gyrX = (GyrX / precission) - calGyrX;
  gyrY = (GyrY / precission) - calGyrY;
  gyrZ = (GyrZ / precission) - calGyrZ;
}

void setup()
{
  Serial.begin(9600);
  setup_mpu(MPU_1);
  calibration(MPU_1, 30);
}

void loop()
{
  smooth(MPU_1, 50);
  Serial.print(">accX:");
  Serial.println(accX);
  Serial.print(">accY:");
  Serial.println(accY);
  Serial.print(">accZ:");
  Serial.println(accZ);
  /*
  Serial.print(">gyrX:");
  Serial.println(gyrX);
  Serial.print(">gyrY:");
  Serial.println(gyrY);
  Serial.print(">gyrZ:");
  Serial.println(gyrZ);*/
}