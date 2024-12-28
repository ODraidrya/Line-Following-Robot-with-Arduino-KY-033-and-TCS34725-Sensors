#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Initialize the TCS34725 sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600); // Start serial communication
  
  // Check if the sensor is detected
  if (!tcs.begin()) {
    Serial.println("TCS34725 not detected! Please check the wiring.");
    while (1); // Infinite loop in case of an error
  }
  
  Serial.println("TCS34725 detected! Displaying sensor data:");
}

void loop() {
  uint16_t r, g, b, c; // Variables for the color values (Red, Green, Blue, Clear)
  
  // Read raw data from the TCS34725 sensor
  tcs.getRawData(&r, &g, &b, &c);

  // Output the sensor data to the serial console
  Serial.print("Red: "); 
  Serial.print(r); 
  Serial.print(" | Green: "); 
  Serial.print(g);
  Serial.print(" | Blue: "); 
  Serial.print(b); 
  Serial.print(" | Clear: "); 
  Serial.println(c);

  delay(500); // Wait 500ms between measurements
}
