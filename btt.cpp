/*
  VeinVeer X1 - Thermal Vein Finder Simulator
  For Arduino Uno / Nano
  This code reads a temperature sensor (LM35 or TMP36) to simulate detecting
  "blood heat" (warmer areas) and visualizes the thermal map on an LED bar
  and a 16x2 LCD.
*/

#include <LiquidCrystal.h>

// Pin Definitions
const int TEMP_SENSOR_PIN = A0;      // Analog pin for temperature sensor (LM35/TMP36)
const int LED_PINS[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12}; // 10 LEDs for thermal visualization
const int NUM_LEDS = 10;
const int BUZZER_PIN = 2;             // Buzzer for "vein found" alert
const int BUTTON_PIN = 13;             // Button to simulate mode toggle

// LCD Pins (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(A1, A2, A3, A4, A5, A0); // Note: A0 reused - adjust if needed

// Temperature Thresholds (in Celsius)
const float BASELINE_TEMP = 30.0;      // Average skin temperature
const float BLOOD_HEAT_DELTA = 0.5;    // Veins are 0.5°C warmer
const float VEIN_TEMP_THRESHOLD = BASELINE_TEMP + BLOOD_HEAT_DELTA;

// System variables
float currentTemperature = 0.0;
float maxTemperature = 0.0;
int currentLedLevel = 0;
bool deepModeActive = false;
unsigned long lastSensorRead = 0;
const int SENSOR_DELAY = 200; // Read sensor every 200ms

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  
  // Initialize LED pins as outputs
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Initialize button with internal pull-up
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("VeinVeer X1");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  
  // Calibration message
  lcd.clear();
  lcd.print("Place on skin");
  lcd.setCursor(0, 1);
  lcd.print("for calibration");
  delay(3000);
  
  // Perform quick calibration
  calibrateSensor();
  
  lcd.clear();
  lcd.print("Ready to scan");
  lcd.setCursor(0, 1);
  lcd.print("Mode: Standard");
}

void loop() {
  // Check for mode toggle button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    toggleMode();
    delay(500); // Debounce
  }
  
  // Read temperature at regular intervals
  if (millis() - lastSensorRead >= SENSOR_DELAY) {
    readTemperature();
    lastSensorRead = millis();
    
    // Update thermal visualization
    updateThermalDisplay();
    
    // Update LCD
    updateLCD();
    
    // Check for vein detection
    checkForVein();
  }
}

void readTemperature() {
  // Read analog value from temperature sensor
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  
  // Convert to voltage (for Arduino, 5V reference, 10-bit ADC)
  float voltage = sensorValue * (5.0 / 1023.0);
  
  // Convert to temperature based on sensor type
  // For LM35: 10mV per °C, so voltage * 100 = temperature °C
  // For TMP36: 10mV per °C + 500mV offset, so (voltage - 0.5) * 100
  currentTemperature = voltage * 100.0; // Assuming LM35
  
  // For TMP36, uncomment the line below and comment the above
  // currentTemperature = (voltage - 0.5) * 100.0;
  
  // Update max temperature if current is higher
  if (currentTemperature > maxTemperature) {
    maxTemperature = currentTemperature;
  }
  
  // Add some random fluctuation for simulation purposes
  // This creates a more dynamic "scanning" effect
  static float simulatedVein = 0.0;
  simulatedVein += 0.1;
  if (simulatedVein > 2 * PI) simulatedVein = 0;
  
  // Simulate a vein crossing the sensor (warmer spot)
  float veinEffect = sin(simulatedVein) * 0.3;
  if (veinEffect > 0.2) {
    currentTemperature += 0.4; // Simulate blood heat
  }
  
  Serial.print("Temp: ");
  Serial.print(currentTemperature);
  Serial.println(" °C");
}

void updateThermalDisplay() {
  // Calculate how many LEDs to light based on temperature
  // Map temperature to LED level (0 to NUM_LEDS)
  // For example: 30°C = 0 LEDs, 35°C = all LEDs
  
  float minTemp = BASELINE_TEMP - 1.0;
  float maxTemp = BASELINE_TEMP + 4.0;
  
  // Constrain temperature to our range
  float constrainedTemp = constrain(currentTemperature, minTemp, maxTemp);
  
  // Map to LED count
  int newLedLevel = map((constrainedTemp * 100), (minTemp * 100), (maxTemp * 100), 0, NUM_LEDS);
  
  // Apply deep mode amplification if active
  if (deepModeActive) {
    newLedLevel = min(newLedLevel + 3, NUM_LEDS); // Boost by 3 LEDs
  }
  
  // Update LEDs if level changed
  if (newLedLevel != currentLedLevel) {
    currentLedLevel = newLedLevel;
    
    // Turn on LEDs up to current level
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i < currentLedLevel) {
        // Brighter for warmer temperatures
        if (i > NUM_LEDS - 4) {
          // Top LEDs (warmest) glow brighter or use different pattern
          analogWrite(LED_PINS[i], 255); // Full brightness
        } else {
          analogWrite(LED_PINS[i], 150); // Medium brightness
        }
      } else {
        digitalWrite(LED_PINS[i], LOW);
      }
    }
  }
}

void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(currentTemperature, 1);
  lcd.print((char)223); // Degree symbol
  lcd.print("C   ");
  
  lcd.setCursor(0, 1);
  if (currentTemperature >= VEIN_TEMP_THRESHOLD) {
    lcd.print("VEIN FOUND!     ");
  } else {
    if (deepModeActive) {
      lcd.print("Mode: Deep      ");
    } else {
      lcd.print("Scanning...     ");
    }
  }
}

void checkForVein() {
  // If temperature indicates a vein (blood heat), beep
  static bool wasVeinDetected = false;
  bool isVeinDetected = (currentTemperature >= VEIN_TEMP_THRESHOLD);
  
  if (isVeinDetected && !wasVeinDetected) {
    // Vein just detected - beep once
    tone(BUZZER_PIN, 1000, 100);
    delay(100);
    tone(BUZZER_PIN, 1500, 100);
  } else if (!isVeinDetected && wasVeinDetected) {
    // Vein lost - short low beep
    tone(BUZZER_PIN, 500, 50);
  }
  
  wasVeinDetected = isVeinDetected;
}

void toggleMode() {
  deepModeActive = !deepModeActive;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode Toggled");
  lcd.setCursor(0, 1);
  
  if (deepModeActive) {
    lcd.print("Deep Tissue ON");
    // Visual feedback - all LEDs blink twice
    for (int j = 0; j < 2; j++) {
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], HIGH);
      }
      delay(200);
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], LOW);
      }
      delay(200);
    }
  } else {
    lcd.print("Standard Mode");
    // Single blink
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], HIGH);
    }
    delay(300);
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], LOW);
    }
  }
  
  delay(1500);
}

void calibrateSensor() {
  lcd.clear();
  lcd.print("Calibrating...");
  
  // Take 10 readings and average for baseline
  float sum = 0;
  for (int i = 0; i < 10; i++) {
    int sensorValue = analogRead(TEMP_SENSOR_PIN);
    float voltage = sensorValue * (5.0 / 1023.0);
    sum += voltage * 100.0; // Assuming LM35
    delay(100);
    
    // Show progress
    lcd.setCursor(0, 1);
    lcd.print("Reading ");
    lcd.print(i + 1);
    lcd.print("/10");
  }
  
  float avgTemp = sum / 10.0;
  
  lcd.clear();
  lcd.print("Calibration OK");
  lcd.setCursor(0, 1);
  lcd.print("Base: ");
  lcd.print(avgTemp, 1);
  lcd.print((char)223);
  lcd.print("C");
  delay(2000);
}