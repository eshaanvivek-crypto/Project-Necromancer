#include <Servo.h>

Servo myServo;
int servoPin = 9;
int buttonPin = 2;  // change this to whatever pin your button is on

void setup() {
  Serial.begin(9600);
  myServo.attach(servoPin);
  pinMode(buttonPin, INPUT_PULLUP);
  myServo.write(0);
  delay(500);
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    // Button pressed - sweep forward
    for (int angle = 0; angle <= 180; angle++) {
      myServo.write(angle);
      delay(15);
    }
  } else {
    // Button not pressed - sweep back
    for (int angle = 180; angle >= 0; angle--) {
      myServo.write(angle);
      delay(15);
    }
  }
}