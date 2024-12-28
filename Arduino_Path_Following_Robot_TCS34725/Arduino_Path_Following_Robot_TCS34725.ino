/**
 * Author: Omar Draidrya
 * Date: 2024/12/06
 * Line-Following Robot with TCS34725.
 */
 
#include <Wire.h>
#include <AFMotor.h>
#include <Adafruit_TCS34725.h>
 
// Initialize TCS34725 sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X);
 
// Motor control pins
#define DIR_A 12
#define DIR_B 13
#define PWM_A 3
#define PWM_B 11
 
// PID parameters
float Kp = 24.0;  // Proportional gain
float Ki = 1.0;   // Integral gain
float Kd = 1.0;   // Derivative gain
int baseSpeed = 60;  // Base speed
 
// Middle value for black/white
int middleValue = 3; // Adjusted based on measured values
 
// Global variables for PID
float error = 0, previousError = 0, sumError = 0;
 
void setup() {
  Serial.begin(9600);
 
  // Initialize TCS34725 sensor
  if (!tcs.begin()) {
    Serial.println("Error: TCS34725 not detected. Check wiring!");
    while (1);
  }
  Serial.println("TCS34725 sensor detected!");
 
  // Configure motor control pins
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
 
  // Perform sensor calibration
  calibrateSensor();
}
 
void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
 
  // Calculate error
  error = middleValue - (float)c ;
 
  // PID calculation
  sumError += error;
  sumError = constrain(sumError, -4, 4); // Limit integral component
 
  if (abs(error) < 2.0) {
    sumError = 0; // Reset for small errors
  }
 
  float differential = error - previousError;
  float correction = Kp * error + Ki * sumError + Kd * differential;
  previousError = error;
 
  // Adjust motor speeds
  int speedLeft = baseSpeed + (int)correction;
  int speedRight = baseSpeed - (int)correction;
 
  speedLeft = constrain(speedLeft, 20, 255);
  speedRight = constrain(speedRight, 20, 255);
 
  controlMotors(speedLeft, speedRight);
}
 
void calibrateSensor() {
  uint16_t r, g, b, c;
 
  // Measure black value
  Serial.println("Calibrating: Measuring black value...");
  controlMotors(-85, 85); // Slightly turn left
  delay(350);
   // Stop motors after calibration
  controlMotors(0, 0);
  delay(50);
 
  tcs.getRawData(&r, &g, &b, &c);
  int blackValue = c;
  Serial.print("Black Value: ");
  Serial.println(blackValue);
 
  // Measure white value
  Serial.println("Calibrating: Measuring white value...");
  controlMotors(85, -85); // Slightly turn right
  delay(500);
    // Stop motors after calibration
  controlMotors(0, 0);
  delay(50);
 
  tcs.getRawData(&r, &g, &b, &c);
  int whiteValue = c;
  Serial.print("White Value: ");
  Serial.println(whiteValue);
 
  // Calculate middle value
  middleValue = (blackValue + whiteValue) / 2;
  Serial.print("Middle Value: ");
  Serial.println(middleValue);
 
  // Stop motors after calibration
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