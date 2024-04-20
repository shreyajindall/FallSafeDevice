#include <Arduino_BMI270_BMM150.h>
#include <ArduinoBLE.h>

BLEService newService("180A");  // creating the service
BLEByteCharacteristic readChar("2A57", BLERead);
BLEByteCharacteristic writeChar("2A58", BLEWrite);

float x, y, z;
int plusThreshold = 100, minusThreshold = -100;
int Fall_detected = 0;
const int RedLEDPin = 2;
const int GreenLEDPin = 3;
const int ButtonPin = 12;
int counter = 0;

void setup() {

  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  pinMode(RedLEDPin, OUTPUT);
  pinMode(GreenLEDPin, OUTPUT);
  pinMode(ButtonPin, INPUT);


  while (!Serial)
    ;
  if (!BLE.begin()) {
    Serial.println("Waiting for ArduinoBLE");
    while (1)
      ;
  }

  BLE.setDeviceName("FallSafe");
  BLE.setLocalName("FallSafe");
  BLE.setAdvertisedService(newService);
  newService.addCharacteristic(readChar);
  BLE.addService(newService);

  readChar.writeValue(0);
  writeChar.writeValue(0);

  BLE.advertise();
  Serial.println("Bluetooth device active");

  digitalWrite(GreenLEDPin, HIGH);
  digitalWrite(RedLEDPin, LOW);
}
void loop() {

  BLEDevice central = BLE.central();  // wait for a BLE central

  if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");
    Serial.println(central.address());  // print the central's BT address

    while (Fall_detected < 0.0)
      ;
    {

      if (digitalRead(ButtonPin) == HIGH) {
        readChar.writeValue(Fall_detected);
        digitalWrite(GreenLEDPin, HIGH);
        digitalWrite(RedLEDPin, LOW);
      }

      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(x, y, z);
      }
      if (y > plusThreshold) {
        Fall_detected = 1;
        Serial.println("Collision front");
        delay(10);
      }
      if (y < minusThreshold) {
        Fall_detected = 1;
        Serial.println("Collision back");
        delay(10);
      }
      if (x < minusThreshold) {
        Fall_detected = 1;
        Serial.println("Collision right");
        delay(10);
      }
      if (x > plusThreshold) {
        Fall_detected = 1;
        Serial.println("Collision left");
        delay(10);
      }
      if (z < minusThreshold) {
        Fall_detected = 1;
        Serial.println("Collision up");
        delay(10);
      }
      if (z > plusThreshold) {
        Fall_detected = 1;
        Serial.println("Collision down");
        delay(10);
      }
      delay(10);
    }
    while (Fall_detected > 0.0) {
      digitalWrite(GreenLEDPin, LOW);
      digitalWrite(RedLEDPin, HIGH);
      counter = 0;
      while (counter < 1000) {
        Serial.println(counter);
        Serial.println(digitalRead(ButtonPin));
        if (digitalRead(ButtonPin) == HIGH) {
          counter = 1000;
        }
        counter++;
        delay(10);
      }

      readChar.writeValue(Fall_detected);
      Fall_detected = 0;
    }
  }

}