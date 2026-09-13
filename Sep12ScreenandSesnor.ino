/*
  Ultrasonic Distance Meter with SSD1306 OLED
  --------------------------------------------
  Hardware:
    - Arduino Uno (or similar)
    - HC-SR04 ultrasonic sensor
    - SSD1306 OLED display (I2C, 128x64 typical)

  Wiring:
    OLED (I2C):
      VCC  -> 5V   (or 3.3V, check your module's label)
      GND  -> GND
      SDA  -> A4   (Uno)
      SCL  -> A5   (Uno)

    HC-SR04:
      VCC  -> 5V
      GND  -> GND
      TRIG -> Pin 9
      ECHO -> Pin 10

  Libraries needed (install via Library Manager):
    - Adafruit GFX Library
    - Adafruit SSD1306
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1   // Reset pin not used
#define SCREEN_ADDRESS 0x3C // Common address; some boards use 0x3D

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int trigPin = 9;
const int echoPin = 10;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // halt if display isn't found
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  delay(500);
}

void loop() {
  long duration;
  float distanceCm;

  // Send a 10us pulse to trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pulse duration (microseconds)
  duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout ~5m max range

  if (duration == 0) {
    // No echo received (out of range or no object)
    showReading(-1);
    Serial.println("Out of range");
  } else {
    // Speed of sound ~ 0.0343 cm/us, divide by 2 for round trip
    distanceCm = duration * 0.0343 / 2.0;
    showReading(distanceCm);
    Serial.print("Distance: ");
    Serial.print(distanceCm);
    Serial.println(" cm");
  }

  delay(200); // small delay between readings
}

void showReading(float distanceCm) {
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
    float distanceFt = distanceCm / 30.48; // 1 ft = 30.48 cm

    display.setTextSize(2);
    display.setCursor(10, 22);
    display.print(distanceCm, 1);
    display.println(" cm");

    display.setCursor(10, 44);
    display.print(distanceFt, 2);
    display.println(" ft");
  }

  display.display();
}