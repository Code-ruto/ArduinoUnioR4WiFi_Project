#include <ArduinoBLE.h>
#include <Servo.h>
#include <string>

Servo MyServo;
int prevAngle = -1;

// UUIDs
const char * deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d014768a1214";
const char * deviceServiceRequestCharachteristicUuid = "19b10001-e8f2-537e-4f6c-d014768a1215";
const char * deviceServiceResponseCharachteristicUuid = "19b10001-e8f2-537e-4f6c-d014768a1216";

BLEService servoService(deviceServiceUuid);
BLEStringCharacteristic servoRequestCharacteristic(deviceServiceRequestCharachteristicUuid, BLERead | BLEWrite, 9);
BLEStringCharacteristic servoResponseCharacteristic(deviceServiceResponseCharachteristicUuid, BLERead | BLEWrite, 9);

// Motor control pins
const int leftMotorForward = 2;
const int leftMotorBackward = 3;
const int rightMotorForward = 4;
const int rightMotorBackward = 5;

// Variable to keep track of the current command
String currentCommand = "S"; // Initialize with "S" to start in a stopped state

void setup() {
  MyServo.attach(9);
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

  Serial.begin(115200);

  // Setup Bluetooth
  BLE.setDeviceName("ARBot");
  BLE.setLocalName("ARBot");

  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth Low Energy Module Failed ~ ");
    while (1);
  }

  BLE.setAdvertisedService(servoService);
  servoService.addCharacteristic(servoRequestCharacteristic);
  servoService.addCharacteristic(servoResponseCharacteristic);
  BLE.addService(servoService);

  servoResponseCharacteristic.writeValue("90");
  BLE.advertise();
  Serial.println("SCANNING START");
}

void moveForward() {
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(rightMotorBackward, LOW);
}

void moveBackward() {
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, HIGH);
}

void turnLeft() {
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, HIGH);
}

void turnRight() {
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(rightMotorBackward, LOW);
}

void stopMoving() {
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.println("CONNECTED");

    while (central.connected()) {
      if (servoRequestCharacteristic.written()) {
        currentCommand = servoRequestCharacteristic.value(); // Update current command
        Serial.println("Command received: " + currentCommand);

        // Execute the command immediately
        if (currentCommand == "F") {
          moveForward();
          servoResponseCharacteristic.writeValue("Moving Forward");
        } else if (currentCommand == "B") {
          moveBackward();
          servoResponseCharacteristic.writeValue("Moving Backward");
        } else if (currentCommand == "L") {
          turnLeft();
          servoResponseCharacteristic.writeValue("Turning Left");
        } else if (currentCommand == "R") {
          turnRight();
          servoResponseCharacteristic.writeValue("Turning Right");
        } else if (currentCommand == "S") {
          stopMoving();
          servoResponseCharacteristic.writeValue("Stopped");
        }
      }
    }

    // Stop moving when the central device disconnects
    stopMoving();
  }
}
