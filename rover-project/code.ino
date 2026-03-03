#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-1234-1234-abcdefabcdef"


// stop timer - good for testing
unsigned long lastCommandTime = 0;  // stores the last time a command was received
const unsigned long timeout = 60000; // 2000 ms = 2 seconds variable - doesnt acc do anyhting yet until used in end if statement.


// Motor pins
const int M1_IN1 = 2;
const int M1_IN2 = 3;
const int M2_IN1 = 4;
const int M2_IN2 = 5;
const int M3_IN1 = 6;
const int M3_IN2 = 7;
const int M4_IN1 = 8;
const int M4_IN2 = 9;
int speedValue = 150; // 0–255, 255 = full speed


// const just ENSURES that this variables value will not change when the program runs even if you made a mistake saying it would
// ALSO THE ARDUINO CAN HARD WIRE THIS AND THEREFORE NOT WASTE ANY RAM FOR IT!!!

// ---------- Movement Functions ----------
/*void moveForward() {
  Serial.println("Moving Forward");
  digitalWrite(M1_IN1, HIGH); digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN1, HIGH); digitalWrite(M2_IN2, LOW);
  digitalWrite(M3_IN1, HIGH); digitalWrite(M3_IN2, LOW);
  digitalWrite(M4_IN1, HIGH); digitalWrite(M4_IN2, LOW);
}

void moveBackward() {
  Serial.println("Moving Backward");
  digitalWrite(M1_IN1, LOW); digitalWrite(M1_IN2, HIGH);
  digitalWrite(M2_IN1, LOW); digitalWrite(M2_IN2, HIGH);
  digitalWrite(M3_IN1, LOW); digitalWrite(M3_IN2, HIGH);
  digitalWrite(M4_IN1, LOW); digitalWrite(M4_IN2, HIGH);
}

void turnLeft() {
  Serial.println("Turning Left");
  // Left motors backward, right motors forward
  digitalWrite(M1_IN1, LOW); digitalWrite(M1_IN2, HIGH);
  digitalWrite(M2_IN1, LOW); digitalWrite(M2_IN2, HIGH);
  digitalWrite(M3_IN1, HIGH); digitalWrite(M3_IN2, LOW);
  digitalWrite(M4_IN1, HIGH); digitalWrite(M4_IN2, LOW);
}

void turnRight() {
  Serial.println("Turning Right");
  // Left motors forward, right motors backward
  digitalWrite(M1_IN1, HIGH); digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN1, HIGH); digitalWrite(M2_IN2, LOW);
  digitalWrite(M3_IN1, LOW); digitalWrite(M3_IN2, HIGH);
  digitalWrite(M4_IN1, LOW); digitalWrite(M4_IN2, HIGH);
}

void stopMoving() {
  Serial.println("Stopping");
  digitalWrite(M1_IN1, LOW); digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN1, LOW); digitalWrite(M2_IN2, LOW);
  digitalWrite(M3_IN1, LOW); digitalWrite(M3_IN2, LOW);
  digitalWrite(M4_IN1, LOW); digitalWrite(M4_IN2, LOW);
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  Serial.println("Rover Ready.");
  Serial.println("Type F, B, L, R, or S");

  // Set motor pins as OUTPUT
  int pins[] = {M1_IN1,M1_IN2,M2_IN1,M2_IN2,M3_IN1,M3_IN2,M4_IN1,M4_IN2};
  for(int i=0;i<8;i++) pinMode(pins[i], OUTPUT);
}

// ---------- Loop ----------
void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == 'F') moveForward();
    else if (command == 'B') moveBackward();
    else if (command == 'L') turnLeft();
    else if (command == 'R') turnRight();
    else if (command == 'S') stopMoving();
  }
}*/

//NOW WITH PWM I.E. VARYING VOLTAGE INPUT FOR SPEED CONTROL REPLACES MAX 3.3V DEPENDING ON SPEED INPUT
 

// Move Forward
void moveForward() {
  Serial.println("Moving Forward");

  analogWrite(M1_IN1, speedValue);
  digitalWrite(M1_IN2, LOW);

  analogWrite(M2_IN1, speedValue);
  digitalWrite(M2_IN2, LOW);

  analogWrite(M3_IN1, speedValue);
  digitalWrite(M3_IN2, LOW);

  analogWrite(M4_IN1, speedValue);
  digitalWrite(M4_IN2, LOW);
}

// Move Backward
void moveBackward() {
  Serial.println("Moving Backward");

  digitalWrite(M1_IN1, LOW);
  analogWrite(M1_IN2, speedValue);

  digitalWrite(M2_IN1, LOW);
  analogWrite(M2_IN2, speedValue);

  digitalWrite(M3_IN1, LOW);
  analogWrite(M3_IN2, speedValue);

  digitalWrite(M4_IN1, LOW);
  analogWrite(M4_IN2, speedValue);
}

// Turn Left
void turnLeft() {
  Serial.println("Turning Left");

  digitalWrite(M1_IN1, LOW);
  analogWrite(M1_IN2, speedValue);

  digitalWrite(M2_IN1, LOW);
  analogWrite(M2_IN2, speedValue);

  analogWrite(M3_IN1, speedValue);
  digitalWrite(M3_IN2, LOW);

  analogWrite(M4_IN1, speedValue);
  digitalWrite(M4_IN2, LOW);
}

// Turn Right
void turnRight() {
  Serial.println("Turning Right");

  analogWrite(M1_IN1, speedValue);
  digitalWrite(M1_IN2, LOW);

  analogWrite(M2_IN1, speedValue);
  digitalWrite(M2_IN2, LOW);

  digitalWrite(M3_IN1, LOW);
  analogWrite(M3_IN2, speedValue);

  digitalWrite(M4_IN1, LOW);
  analogWrite(M4_IN2, speedValue);
}

// Stop
void stopMoving() {
  Serial.println("Stopping");

  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, LOW);

  digitalWrite(M2_IN1, LOW);
  digitalWrite(M2_IN2, LOW);

  digitalWrite(M3_IN1, LOW);
  digitalWrite(M3_IN2, LOW);

  digitalWrite(M4_IN1, LOW);
  digitalWrite(M4_IN2, LOW);
}

class RoverCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {

    String value = pCharacteristic->getValue();   // <-- FIXED

    if (value.length() > 0) {

      lastCommandTime = millis();

      char firstChar = value.charAt(0);

      if (isDigit(firstChar)) {

        int inputSpeed = value.toInt();   // <-- FIXED

        if (inputSpeed >= 0 && inputSpeed <= 255) {
          speedValue = inputSpeed;
        }

      } else {

        char command = firstChar;

        if (command == 'F') moveForward();
        else if (command == 'B') moveBackward();
        else if (command == 'L') turnLeft();
        else if (command == 'R') turnRight();
        else if (command == 'S') stopMoving();
      }
    }
  }
};


// ---------- Setup ----------
void setup() { 
  /*Serial.begin(115200); 
  Serial.println("Rover Ready. Enter speed 0-255");
  Serial.println("Type F, B, L, R, or S"); 
  //Makes array and sets motor pins as OUTPUT
  int pins[] = {M1_IN1,M1_IN2,M2_IN1,M2_IN2,M3_IN1,M3_IN2,M4_IN1,M4_IN2}; 
  for(int i=0;i<8;i++) pinMode(pins[i], OUTPUT);
  */ 
  //NOW WITH SERIAL BLUETOOTH (LE)
   Serial.begin(115200);

  int pins[] = {M1_IN1,M1_IN2,M2_IN1,M2_IN2,M3_IN1,M3_IN2,M4_IN1,M4_IN2};
  for(int i=0;i<8;i++) pinMode(pins[i], OUTPUT);

  BLEDevice::init("RoverESP32C3");

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new RoverCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  Serial.println("BLE Rover Ready"); 
}

// ----------- Loop -----------
bool isStopped = false;
void loop() {

  /*if (SerialBT.available() > 0) {

    lastCommandTime = millis();

    char firstChar = SerialBT.peek();   // <-- FIXED

    if (isDigit(firstChar)) {

      int inputSpeed = SerialBT.parseInt();  // <-- FIXED

      if (inputSpeed >= 0 && inputSpeed <= 255) {
        speedValue = inputSpeed;
        SerialBT.print("Speed set to: ");    // <-- FIXED
        SerialBT.println(speedValue);
      } else {
        SerialBT.println("Invalid speed! Enter 0-255.");
      }

    } else {

      char command = SerialBT.read();   // <-- FIXED

      if (command == 'F') moveForward();
      else if (command == 'B') moveBackward();
      else if (command == 'L') turnLeft();
      else if (command == 'R') turnRight();
      else if (command == 'S') stopMoving();
    }
  }*/


  //All command handling now happens inside the BLE callback.
  if (millis() - lastCommandTime > timeout) {
    if (!isStopped) {
      stopMoving();
      isStopped = true;  // only call stop once
    }
  } else {
    isStopped = false; // reset flag if a new command comes in
  }

  delay(50);
}

