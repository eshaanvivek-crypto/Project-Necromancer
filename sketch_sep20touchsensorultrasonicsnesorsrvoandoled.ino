#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myServo;

const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 6;
const int touchPin = 7;

const int servoMinAngle = 0;
const int servoMaxAngle = 90;
const int sweepStepDelay = 15; // ms between each degree step — lower = faster sweep

int currentAngle = 0;
int sweepDirection = 1; // 1 = increasing, -1 = decreasing
unsigned long lastSweepTime = 0;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(touchPin, INPUT);

  myServo.attach(servoPin);
  myServo.write(currentAngle);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  delay(500);
}

void loop() {
  long duration;
  float distanceCm;
  bool touched = digitalRead(touchPin) == HIGH;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    showReading(-1, touched);
  } else {
    distanceCm = duration * 0.0343 / 2.0;
    showReading(distanceCm, touched);
  }

  // Sweep the servo ONLY while touched
  if (touched) {
    unsigned long now = millis();
    if (now - lastSweepTime >= sweepStepDelay) {
      lastSweepTime = now;
      currentAngle += sweepDirection;

      if (currentAngle >= servoMaxAngle) {
        currentAngle = servoMaxAngle;
        sweepDirection = -1;
      } else if (currentAngle <= servoMinAngle) {
        currentAngle = servoMinAngle;
        sweepDirection = 1;
      }

      myServo.write(currentAngle);
    }
  }
  // if not touched, do nothing — servo holds its current position
}

void showReading(float distanceCm, bool touched) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Ultrasonic Distance");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  if (distanceCm < 0) {
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("Out of");
    display.setCursor(10, 45);
    display.println("range");
  } else {
    float distanceFt = distanceCm / 30.48;

    display.setTextSize(2);
    display.setCursor(10, 22);
    display.print(distanceCm, 1);
    display.println(" cm");

    display.setCursor(10, 44);
    display.print(distanceFt, 2);
    display.println(" ft");
  }

  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print(touched ? "Rotating" : "Stopped");

  display.display();
}