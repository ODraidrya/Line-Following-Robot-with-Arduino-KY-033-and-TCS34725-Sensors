#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Initialize TCS34725 sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X);

// Motor control pins
#define DIR_A 12
#define DIR_B 13
#define PWM_A 3
#define PWM_B 11

// PID parameters
float Kp = 27.0;  // Proportional gain
float Ki = 0.5;   // Integral gain
float Kd = 2.5;   // Derivative gain
int baseSpeed = 55;  // Base speed

// Middle value for black/white
int middleValue = 3; // Adjusted based on measured values

// Global variables for PID
float error = 0, previousError = 0, sumError = 0;

void setup() {
  Serial.begin(9600);

  // Initialize TCS34725 sensor
  if (!tcs.begin()) {
    Serial.println("TCS34725 not detected!");
    while (1);
  }
  Serial.println("TCS34725 detected!");

  // Configure pins
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);

  // Perform calibration
  calibrateSensor();
}

void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Calculate error
  error = (float)c - middleValue;

  // PID calculation
  sumError += error;
  sumError = constrain(sumError, -4, 4); // Limit integral component

  // Reset sumError for small errors
  if (abs(error) < 0.5) {
    sumError = 0;
  }

  float differential = error - previousError;
  float correction = Kp * error + Ki * sumError + Kd * differential;

  previousError = error;

  // Adjust speed based on correction
  int speedLeft = baseSpeed + (int)correction;
  int speedRight = baseSpeed - (int)correction;

  // Limit speed
  speedLeft = constrain(speedLeft, 20, 255);
  speedRight = constrain(speedRight, 20, 255);

  // Control motors
  controlMotors(speedLeft, speedRight);

  // Debugging
  Serial.print("Clear: ");
  Serial.print(c);
  Serial.print(" | Error: ");
  Serial.print(error);
  Serial.print(" | SumError: ");
  Serial.print(sumError);
  Serial.print(" | Correction: ");
  Serial.print(correction);
  Serial.print(" | Speed Left: ");
  Serial.print(speedLeft);
  Serial.print(" | Speed Right: ");
  Serial.println(speedRight);

  delay(10); // Short delay for stability
}

void calibrateSensor() {
  uint16_t r, g, b, c;

  // Measure dark area
  Serial.println("Measuring dark area...");
  controlMotors(-85, 85); // Slightly turn left
  delay(350);
  tcs.getRawData(&r, &g, &b, &c);
  int blackValue = c;
  Serial.print("Black Value: ");
  Serial.println(blackValue);

  // Measure bright area
  Serial.println("Measuring bright area...");
  controlMotors(85, -85); // Slightly turn right
  delay(700);
  tcs.getRawData(&r, &g, &b, &c);
  int whiteValue = c;
  Serial.print("White Value: ");
  Serial.println(whiteValue);

  // Calculate middle value
  middleValue = (blackValue + whiteValue) / 2;
  Serial.print("Middle Value: ");
  Serial.println(middleValue);

  // Stop motors
  controlMotors(0, 0);
}

void controlMotors(int speedLeft, int speedRight) {
  if (speedLeft >= 0) {
    digitalWrite(DIR_A, HIGH);
  } else {
    digitalWrite(DIR_A, LOW);
    speedLeft = abs(speedLeft);
  }

  if (speedRight >= 0) {
    digitalWrite(DIR_B, HIGH);
  } else {
    digitalWrite(DIR_B, LOW);
    speedRight = abs(speedRight);
  }

  analogWrite(PWM_A, speedLeft);
  analogWrite(PWM_B, speedRight);
}
