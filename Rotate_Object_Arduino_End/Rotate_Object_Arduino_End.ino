#include <Wire.h>
#include <SoftWire.h>
#include <SPI.h>
#include <SD.h>
#include <Kalman.h>

// SoftWire pins for MPU6050 (change if needed)
#define MPU_SDA_PIN 3
#define MPU_SCL_PIN 2

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

// Create SoftWire object for MPU6050
SoftWire sw(MPU_SDA_PIN, MPU_SCL_PIN);

#define SW_TX_BUF_LEN 32
#define SW_RX_BUF_LEN 32
uint8_t sw_tx_buf[SW_TX_BUF_LEN];
uint8_t sw_rx_buf[SW_RX_BUF_LEN];

void requestEvent() {
  struct SensorData {
    int16_t Xrot;
    int16_t Yrot;
    uint8_t AccX;
    uint8_t AccY;
  } data;

  data.Xrot = (int16_t)Xrot;
  data.Yrot = (int16_t)Yrot;
  data.AccX = (int8_t)(AccX * 100);
  data.AccY = (int8_t)(AccY * 100);

  Serial.print("I2C Data Sent: ");
  Serial.print("Xrot: "); Serial.print(data.Xrot); Serial.print(", ");
  Serial.print("Yrot: "); Serial.print(data.Yrot); Serial.print(", ");
  Serial.print("AccX: "); Serial.print(data.AccX); Serial.print(", ");
  Serial.print("AccY: "); Serial.print(data.AccY); Serial.println();

  Wire.write((uint8_t*)&data, sizeof(data));
}

void setup() {
  Serial.begin(115200);

  // Set SoftWire buffers
  sw.setTxBuffer(sw_tx_buf, SW_TX_BUF_LEN);
  sw.setRxBuffer(sw_rx_buf, SW_RX_BUF_LEN);

  // Now begin SoftWire
  sw.begin();

  // Hardware I2C as slave for Tiny4FSK
  Wire.begin(9);
  Wire.onRequest(requestEvent);

  // MPU6050 setup via SoftWire
  sw.beginTransmission(MPU);
  sw.write(0x6B); // Power management register
  sw.write(0x00); // Wake up MPU6050
  sw.endTransmission(true);

  sw.beginTransmission(MPU);
  sw.write(0x1C); // ACCEL_CONFIG
  sw.write(0x10); // +/- 8g
  sw.endTransmission(true);

  sw.beginTransmission(MPU);
  sw.write(0x1B); // GYRO_CONFIG
  sw.write(0x10); // 1000 deg/s
  sw.endTransmission(true);

  delay(20);

  // Read initial accelerometer values
  sw.beginTransmission(MPU);
  sw.write(0x3B);
  sw.endTransmission(false);
  sw.requestFrom(MPU, 6, true);
  AccX = (sw.read() << 8 | sw.read()) / 16384.0;
  AccY = (sw.read() << 8 | sw.read()) / 16384.0;
  AccZ = (sw.read() << 8 | sw.read()) / 16384.0;

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
  // === Read accelerometer via SoftWire ===
  sw.beginTransmission(MPU);
  sw.write(0x3B);
  sw.endTransmission(false);
  delay(2);
  sw.requestFrom(MPU, 6, true);
  AccX = (sw.read() << 8 | sw.read()) / 16384.0;
  AccY = (sw.read() << 8 | sw.read()) / 16384.0;
  AccZ = (sw.read() << 8 | sw.read()) / 16384.0;

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI);
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI);

  // === Read gyroscope via SoftWire ===
  sw.beginTransmission(MPU);
  sw.write(0x43);
  sw.endTransmission(false);
  delay(2);
  sw.requestFrom(MPU, 6, true);
  GyroX = (sw.read() << 8 | sw.read()) / 131.0;
  GyroY = (sw.read() << 8 | sw.read()) / 131.0;
  GyroZ = (sw.read() << 8 | sw.read()) / 131.0;

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
}