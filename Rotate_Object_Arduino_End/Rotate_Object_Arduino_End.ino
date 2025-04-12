#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Kalman.h>

const int chipSelect = 10;
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float Xrot, Yrot, Zrot;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

Kalman kalmanX;  // Kalman filter instance for X-axis
Kalman kalmanY;  // Kalman filter instance for Y-axis

void requestEvent() {
  Wire.beginTransmission(9);
  Wire.write((byte)Xrot);
  Wire.write(",");
  Wire.write((byte)Yrot);
  Wire.write(",");
  Wire.write((byte)Zrot);
  Wire.write(",");
  Wire.write((byte)AccX * 101);
  Wire.write(",");
  Wire.write((byte)AccY * 101);
  Wire.write(",");
  Wire.write((byte)AccZ * 101);
  Wire.endTransmission();
  delay(500);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.begin(9);                     // Initialize communication
  Wire.onRequest(requestEvent);      // Register request event
  Wire.beginTransmission(MPU);       // Start communication with MPU6050
  Wire.write(0x6B);                  // Register 6B
  Wire.write(0x00);                  // Reset
  Wire.endTransmission(true);
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  // Talk to ACCEL_CONFIG register
  Wire.write(0x10);                  // +/- 8g full scale range
  Wire.endTransmission(true);
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                  // Talk to GYRO_CONFIG register
  Wire.write(0x10);                  // 1000 deg/s full scale
  Wire.endTransmission(true);
  delay(20);
  calculate_IMU_error();
  delay(20);
  SD.begin(chipSelect);

  // Initialize Kalman filter with initial angle
  kalmanX.setAngle(accAngleX);
  kalmanY.setAngle(accAngleY);
  gyroAngleX = accAngleX;
  gyroAngleY = accAngleY;
}

void loop() {
  // === Read accelerometer data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58;
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58;

  // === Read gyroscope data === //
  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000;

  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;

  GyroX = GyroX + 0.56;
  GyroY = GyroY - 2;
  GyroZ = GyroZ + 0.79;

  gyroAngleX = gyroAngleX + GyroX * elapsedTime;
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  Zrot =  Xrot + GyroZ * elapsedTime;

  // === Kalman filter === //
  Xrot = kalmanX.getAngle(accAngleX, GyroX, elapsedTime);
  Yrot = kalmanY.getAngle(accAngleY, GyroY, elapsedTime);

  // Print values to Serial Monitor
  Serial.print((int)Xrot);
  Serial.print(", ");
  Serial.print((int)Yrot);
  Serial.print(", ");
  Serial.print((int)Zrot);
  Serial.print(", ");
  Serial.print((int)(AccX));
  Serial.print(", ");
  Serial.print((int)(AccY));
  Serial.print(", ");
  Serial.print((int)(AccZ));
  Serial.println(", ");

  // Save data to SD card
  File dataFile = SD.open("DATA.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print((String)Xrot);
    dataFile.print(", ");
    dataFile.print((String)Yrot);
    dataFile.print(", ");
    dataFile.print((String)Zrot);
    dataFile.print(", ");
    dataFile.print((String)AccX);
    dataFile.print(", ");
    dataFile.print((String)AccY);
    dataFile.print(", ");
    dataFile.print((String)AccZ);
    dataFile.println(", ");
    dataFile.close();
  } else {
    Serial.println("error opening data.txt");
  }
}

void calculate_IMU_error() {
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccErrorX += (atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI);
    AccErrorY += (atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI);
    c++;
  }
  AccErrorX /= 200;
  AccErrorY /= 200;
  c = 0;

  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    GyroErrorX += (GyroX / 131.0);
    GyroErrorY += (GyroY / 131.0);
    GyroErrorZ += (GyroZ / 131.0);
    c++;
  }
  GyroErrorX /= 200;
  GyroErrorY /= 200;
  GyroErrorZ /= 200;

  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}