/**
 * Servo Pulse Width Calibrator - Configuration-Based
 * 
 * Find the optimal min/max pulse widths for your specific SG90 servos
 * NOW WITH SAFETY LIMITS from CalibratorConfig.h
 * 
 * IMPORTANT: Edit CalibratorConfig.h to set:
 * - Which servo you're testing (SERVO_CHANNEL)
 * - Safety boundaries (ABSOLUTE_MIN/MAX)
 * - Starting values (STARTING_MIN/MAX)
 * 
 * The calibrator will NOT allow you to exceed ABSOLUTE limits!
 * This protects your mechanism from damage.
 * 
 * CONTROLS:
 * - Send '1' in Serial Monitor → DECREASE min pulse (servo goes further one direction)
 * - Send '2' in Serial Monitor → INCREASE min pulse (servo backs off)
 * - Send '3' in Serial Monitor → DECREASE max pulse (servo backs off)
 * - Send '4' in Serial Monitor → INCREASE max pulse (servo goes further)
 * - Send '+' in Serial Monitor → INCREASE step size by 5 (faster adjustments)
 * - Send '-' in Serial Monitor → DECREASE step size by 5 (finer control)
 * - Send 't' in Serial Monitor → TEST sweep with current values
 * - Send 's' in Serial Monitor → SAVE/display current values
 * 
 * STEP SIZE CONTROL:
 * - Default step size: Set in CalibratorConfig.h
 * - Increase to 10-20 for coarse adjustments (finding general range)
 * - Decrease to 1-2 for fine-tuning (precision calibration)
 * - Range: 1-50 (automatically constrained)
 * 
 * GOAL: Find pulse widths where servo reaches full physical range without buzzing
 * 
 * Hardware:
 * - Arduino UNO
 * - Adafruit Servo Shield (address in CalibratorConfig.h)
 * - 1x SG90 Servo on channel specified in CalibratorConfig.h
 * - 5V power supply
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "CalibratorConfig.h"  // ALL CONFIGURATION HERE

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(SERVO_SHIELD_ADDRESS);

// Starting values (loaded from config, user can adjust)
int servoMin = STARTING_MIN_PULSE;
int servoMax = STARTING_MAX_PULSE;

// Adjustment step size (user can change this)
int stepSize = DEFAULT_STEP_SIZE;

bool servoShieldFound = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\n\n========================================"));
  Serial.println(F("Servo Pulse Width Calibrator"));
  Serial.println(F("Configuration-Based Safety System"));
  Serial.println(F("========================================\n"));
  
  // Display loaded configuration
  Serial.println(F("Configuration from CalibratorConfig.h:"));
  Serial.println(F("----------------------------------------"));
  Serial.print(F("Servo Channel: "));
  Serial.println(SERVO_CHANNEL);
  Serial.print(F("Servo Name: "));
  Serial.println(SERVO_NAME);
  Serial.print(F("Shield Address: 0x"));
  Serial.println(SERVO_SHIELD_ADDRESS, HEX);
  Serial.println();
  Serial.println(F("Safety Boundaries:"));
  Serial.print(F("  Absolute MIN: "));
  Serial.print(ABSOLUTE_MIN_PULSE);
  Serial.println(F(" (HARD FLOOR)"));
  Serial.print(F("  Absolute MAX: "));
  Serial.print(ABSOLUTE_MAX_PULSE);
  Serial.println(F(" (HARD CEILING)"));
  Serial.println();
  Serial.println(F("Starting Values:"));
  Serial.print(F("  MIN: "));
  Serial.println(servoMin);
  Serial.print(F("  MAX: "));
  Serial.println(servoMax);
  Serial.print(F("  Step: "));
  Serial.println(stepSize);
  Serial.println(F("----------------------------------------\n"));
  
  // Initialize servo shield
  Serial.print(F("Initializing servo shield... "));
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);
  
  Wire.beginTransmission(SERVO_SHIELD_ADDRESS);
  if (Wire.endTransmission() != 0) {
    Serial.println(F("FAILED!"));
    Serial.print(F("Shield not found at 0x"));
    Serial.println(SERVO_SHIELD_ADDRESS, HEX);
    while(1);
  }
  Serial.println(F("OK\n"));
  
  printInstructions();
  
  // Move to center
  int centerPulse = (servoMin + servoMax) / 2;
  pwm.setPWM(SERVO_CHANNEL, 0, centerPulse);
  
  Serial.println(F("⚠️  SAFETY: Calibrator limited to"));
  Serial.print(F("   "));
  Serial.print(ABSOLUTE_MIN_PULSE);
  Serial.print(F(" - "));
  Serial.println(ABSOLUTE_MAX_PULSE);
  Serial.println(F("   Cannot exceed these limits!\n"));
  
  printCurrentValues();
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    switch (command) {
      case '1':  // Decrease MIN (servo goes further in one direction)
        servoMin -= stepSize;
        servoMin = constrain(servoMin, ABSOLUTE_MIN_PULSE, ABSOLUTE_MAX_PULSE);
        Serial.print(F("MIN decreased to: "));
        Serial.println(servoMin);
        if (servoMin == ABSOLUTE_MIN_PULSE) {
          Serial.println(F("⚠️  Hit ABSOLUTE MIN limit!"));
        }
        testMinPosition();
        break;
        
      case '2':  // Increase MIN (servo backs off)
        servoMin += stepSize;
        servoMin = constrain(servoMin, ABSOLUTE_MIN_PULSE, ABSOLUTE_MAX_PULSE);
        Serial.print(F("MIN increased to: "));
        Serial.println(servoMin);
        testMinPosition();
        break;
        
      case '3':  // Decrease MAX (servo backs off)
        servoMax -= stepSize;
        servoMax = constrain(servoMax, ABSOLUTE_MIN_PULSE, ABSOLUTE_MAX_PULSE);
        Serial.print(F("MAX decreased to: "));
        Serial.println(servoMax);
        testMaxPosition();
        break;
        
      case '4':  // Increase MAX (servo goes further)
        servoMax += stepSize;
        servoMax = constrain(servoMax, ABSOLUTE_MIN_PULSE, ABSOLUTE_MAX_PULSE);
        Serial.print(F("MAX increased to: "));
        Serial.println(servoMax);
        if (servoMax == ABSOLUTE_MAX_PULSE) {
          Serial.println(F("⚠️  Hit ABSOLUTE MAX limit!"));
        }
        testMaxPosition();
        break;
        
      case '+':  // Increase step size
      case '=':  // Also accept = (same key as + without shift)
        stepSize += 5;
        stepSize = constrain(stepSize, 1, 50);  // Limit to reasonable range
        Serial.print(F("Step size increased to: "));
        Serial.println(stepSize);
        break;
        
      case '-':  // Decrease step size
      case '_':  // Also accept _ (same key as - with shift)
        stepSize -= 5;
        stepSize = constrain(stepSize, 1, 50);  // Limit to reasonable range
        Serial.print(F("Step size decreased to: "));
        Serial.println(stepSize);
        break;
        
      case 't':  // Test sweep
      case 'T':
        Serial.println(F("\nTesting full sweep..."));
        testSweep();
        break;
        
      case 's':  // Save/display values
      case 'S':
        displayFinalValues();
        break;
        
      case 'h':  // Help
      case 'H':
      case '?':
        printInstructions();
        break;
        
      default:
        // Ignore newlines and other chars
        break;
    }
  }
}

void printInstructions() {
  Serial.println(F("========================================"));
  Serial.println(F("INSTRUCTIONS"));
  Serial.println(F("========================================"));
  Serial.println(F("Type commands in Serial Monitor:"));
  Serial.println(F(""));
  Serial.println(F("  1 = Decrease MIN (go further)"));
  Serial.println(F("  2 = Increase MIN (back off)"));
  Serial.println(F("  3 = Decrease MAX (back off)"));
  Serial.println(F("  4 = Increase MAX (go further)"));
  Serial.println(F(""));
  Serial.println(F("  + = Increase step size (+5)"));
  Serial.println(F("  - = Decrease step size (-5)"));
  Serial.println(F(""));
  Serial.println(F("  t = Test full sweep"));
  Serial.println(F("  s = Show/save current values"));
  Serial.println(F("  h = Show this help"));
  Serial.println(F(""));
  Serial.println(F("GOAL: Adjust until servo reaches"));
  Serial.println(F("      full range WITHOUT buzzing"));
  Serial.println(F("========================================\n"));
}

void printCurrentValues() {
  Serial.println(F("\n--- Current Values ---"));
  Serial.print(F("MIN pulse: "));
  Serial.println(servoMin);
  Serial.print(F("MAX pulse: "));
  Serial.println(servoMax);
  Serial.print(F("Range: "));
  Serial.println(servoMax - servoMin);
  Serial.print(F("Step size: "));
  Serial.println(stepSize);
  Serial.println(F(""));
  Serial.print(F("Safety limits: "));
  Serial.print(ABSOLUTE_MIN_PULSE);
  Serial.print(F(" - "));
  Serial.println(ABSOLUTE_MAX_PULSE);
  Serial.println(F("---------------------\n"));
}

void testMinPosition() {
  Serial.println(F("Moving to MIN position..."));
  pwm.setPWM(SERVO_CHANNEL, 0, servoMin);
  delay(1000);
  
  Serial.println(F("Listen: Does servo buzz/struggle?"));
  Serial.println(F("  YES → Press '2' to back off"));
  Serial.println(F("  NO → Press '1' to go further\n"));
}

void testMaxPosition() {
  Serial.println(F("Moving to MAX position..."));
  pwm.setPWM(SERVO_CHANNEL, 0, servoMax);
  delay(1000);
  
  Serial.println(F("Listen: Does servo buzz/struggle?"));
  Serial.println(F("  YES → Press '3' to back off"));
  Serial.println(F("  NO → Press '4' to go further\n"));
}

void testSweep() {
  // Sweep MIN to MAX
  Serial.println(F("Sweeping MIN → MAX"));
  for (int pulse = servoMin; pulse <= servoMax; pulse += 10) {
    pwm.setPWM(SERVO_CHANNEL, 0, pulse);
    delay(50);
  }
  
  delay(500);
  
  // Sweep MAX to MIN
  Serial.println(F("Sweeping MAX → MIN"));
  for (int pulse = servoMax; pulse >= servoMin; pulse -= 10) {
    pwm.setPWM(SERVO_CHANNEL, 0, pulse);
    delay(50);
  }
  
  // Return to center
  int centerPulse = (servoMin + servoMax) / 2;
  pwm.setPWM(SERVO_CHANNEL, 0, centerPulse);
  
  Serial.println(F("Sweep complete!\n"));
  Serial.println(F("Did it reach full range without buzzing?"));
  Serial.println(F("  YES → Press 's' to save these values"));
  Serial.println(F("  NO → Adjust with 1/2/3/4 and test again\n"));
}

void displayFinalValues() {
  Serial.println(F("\n\n========================================"));
  Serial.println(F("CALIBRATED VALUES FOR YOUR SERVO"));
  Serial.println(F("========================================"));
  Serial.println(F(""));
  Serial.println(F("Use these values in your code:"));
  Serial.println(F(""));
  Serial.print(F("#define SERVOMIN  "));
  Serial.println(servoMin);
  Serial.print(F("#define SERVOMAX  "));
  Serial.println(servoMax);
  Serial.println(F(""));
  Serial.println(F("Or for Configuration.h:"));
  Serial.println(F(""));
  Serial.print(F("MIN = "));
  Serial.println(servoMin);
  Serial.print(F("MAX = "));
  Serial.println(servoMax);
  Serial.print(F("Range = "));
  Serial.println(servoMax - servoMin);
  Serial.println(F(""));
  Serial.println(F("========================================"));
  Serial.println(F("Write these values down!"));
  Serial.println(F("You'll need them for each servo channel."));
  Serial.println(F("========================================\n"));
}
