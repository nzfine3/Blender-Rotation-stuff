#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Kalman.h>

const int chipSelect = 10;
const int MPU = 0x68; // MPU6050 I2C address

float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY;
float elapsedTime, currentTime, previousTime;
float Xrot, Yrot, Zrot;

unsigned long lastSDWriteTime = 0;
const unsigned long sdWriteInterval = 100; // in milliseconds

Kalman kalmanX;
Kalman kalmanY;

void requestEvent() {
  // Prepare data to send via I2C
  byte data[4];
  data[0] = (int16_t)Xrot;
  data[1] = (int16_t)Yrot;
  data[2] = (byte)(AccX * 100);
  data[3] = (byte)(AccY * 100);

  // Print the data to the Serial Monitor
  Serial.print("I2C Data Sent: ");
  Serial.print("Xrot: "); Serial.print((int)data[0]); Serial.print(", ");
  Serial.print("Yrot: "); Serial.print((int)data[1]); Serial.print(", ");
  Serial.print("AccX: "); Serial.print((int)data[2]); Serial.print(", ");
  Serial.print("AccY: "); Serial.print((int)data[3]); Serial.print(", ");

  // Send data via I2C
  Wire.write(data, sizeof(data));
}

void setup() {
  Serial.begin(115200);

  Wire.begin(9); // Join I2C bus as slave address 9
  Wire.onRequest(requestEvent); // Register I2C response handler

  Wire.beginTransmission(MPU);
  Wire.write(0x6B); // Power management register
  Wire.write(0x00); // Wake up MPU6050
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU);
  Wire.write(0x1C); // ACCEL_CONFIG
  Wire.write(0x10); // +/- 8g
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU);
  Wire.write(0x1B); // GYRO_CONFIG
  Wire.write(0x10); // 1000 deg/s
  Wire.endTransmission(true);

  delay(20);

  // Read initial accelerometer values
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI);
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI);

  kalmanX.setAngle(accAngleX);
  kalmanY.setAngle(accAngleY);
  Xrot = accAngleX;
  Yrot = accAngleY;

  previousTime = millis();

  // SD card init
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("Initialized.");
}

void loop() {
  // === Read accelerometer ===
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI);
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI);

  // === Read gyroscope ===
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;

  // Gyro bias correction (adjust as needed)
  GyroX += 0.56;
  GyroY -= 2;
  GyroZ += 0.79;

  // === Calculate rotation ===
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  Xrot = kalmanX.getAngle(accAngleX, GyroX, elapsedTime);
  Yrot = kalmanY.getAngle(accAngleY, GyroY, elapsedTime);

  // === Save to SD card ===
  if (millis() - lastSDWriteTime >= sdWriteInterval) {
    lastSDWriteTime = millis();
    File dataFile = SD.open("DATA.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.print(Xrot); dataFile.print(", ");
      dataFile.print(Yrot); dataFile.print(", ");
      dataFile.print(AccX); dataFile.print(", ");
      dataFile.print(AccY); dataFile.print(", ");
      dataFile.print(AccZ); dataFile.println();
      dataFile.close();
    } else {
      Serial.println("Error opening DATA.txt");
    }
  }
  Serial.print(Xrot); Serial.print(", ");
  Serial.print(Yrot); Serial.print(", ");
  Serial.print(Zrot); Serial.print(", ");
  Serial.print(AccX); Serial.print(", ");
  Serial.print(AccY); Serial.print(", ");
  Serial.print(AccZ); Serial.println();
  delay(10);
}