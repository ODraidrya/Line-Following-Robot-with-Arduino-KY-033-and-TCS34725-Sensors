// Motor control pins
#define DIR_L 12  // Left motor direction
#define DIR_R 13  // Right motor direction
#define PWM_L 3   // Left motor speed (PWM)
#define PWM_R 11  // Right motor speed (PWM)

// Sensor pins
int sensorLeft = 2;    // Left sensor
int sensorRight = 4;   // Right sensor

void setup() {
  // Configure motor pins as outputs
  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);
  pinMode(PWM_L, OUTPUT);
  pinMode(PWM_R, OUTPUT);

  // Configure sensor pins as inputs
  pinMode(sensorLeft, INPUT);
  pinMode(sensorRight, INPUT);

  // Serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read sensor values
  int left = digitalRead(sensorLeft);
  int right = digitalRead(sensorRight);

  // Output sensor data (debugging)
  Serial.print("Sensor Left: ");
  Serial.print(left);
  Serial.print("  |  Sensor Right: ");
  Serial.println(right);

  // Control logic
  if (left == 1 && right == 1) {
    // Drive straight
    controlMotors(HIGH, HIGH, 105, 105);
  } else if (left == 1 && right == 0) {
    // Turn left
    controlMotors(HIGH, LOW, 105, 85);
  } else if (left == 0 && right == 1) {
    // Turn right
    controlMotors(LOW, HIGH, 85, 105);
  } else {
    // Line lost – stop motors
    // Rotate in place to find the line
    controlMotors(HIGH, LOW, 70, 70);  // Left motor forward, right motor backward
    // delay(500);  // Brief rotation
    // controlMotors(LOW, LOW, 0, 0);
  }
}

// Function to control motors
void controlMotors(int directionL, int directionR, int speedL, int speedR) {
  digitalWrite(DIR_L, directionL);
  digitalWrite(DIR_R, directionR);
  analogWrite(PWM_L, speedL);
  analogWrite(PWM_R, speedR);
}
