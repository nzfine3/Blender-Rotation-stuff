 #include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin(9); // Set this Arduino's I2C address to 9
  Wire.onReceive(receiveEvent);
}

void loop() {
  // Nothing needed here; everything happens in receiveEvent
}

void receiveEvent(int bytes) {
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print(c); // Print received characters
  }
  Serial.println(); // Newline after each transmission
}