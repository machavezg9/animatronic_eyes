/**
 * Animatronic Eyes - Enhanced Integration Test
 * With Startup Animation and Idle Behaviors
 * 
 * FEATURES:
 * - Startup animation (eyes close, open, look around, center)
 * - Idle animation (5 random movement sequences when inactive)
 * - Auto-blink during idle
 * - Manual nunchuck control
 * - Smooth eyelid blinks
 * 
 * IMPORTANT: All servo limits are defined in EyeConfig.h
 * Edit EyeConfig.h to change calibration values, NOT this file!
 * 
 * Hardware:
 * - Arduino UNO
 * - Adafruit Servo Shield (address set in EyeConfig.h)
 * - Wii Nunchuck
 * - 6x SG90 Servos (calibrated)
 * - 5V 4A power supply
 * 
 * CONTROLS:
 * - Joystick: Move eyes (exits idle mode)
 * - Z Button: Blink (exits idle mode)
 * - C Button: Return to center (exits idle mode)
 * - Idle 15 seconds: Automatic animation starts
 */

#include <Wire.h>
#include <WiiChuck.h>
#include <Adafruit_PWMServoDriver.h>
#include "EyeConfig.h"  // ALL CONFIGURATION VALUES ARE HERE

// ============================================================================
// OBJECTS
// ============================================================================

Accessory nunchuck;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(SERVO_SHIELD_ADDRESS);

// ============================================================================
// ANIMATION STATES
// ============================================================================

enum SystemState {
  STATE_STARTUP,           // Running startup animation
  STATE_ACTIVE,            // User controlling with nunchuck
  STATE_IDLE               // Idle animation mode
};

enum StartupPhase {
  STARTUP_CLOSE_EYES,
  STARTUP_HOLD_CLOSED,
  STARTUP_OPEN_EYES,
  STARTUP_LOOK_AROUND,
  STARTUP_CENTER,
  STARTUP_COMPLETE
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// System state
SystemState currentState = STATE_STARTUP;
StartupPhase startupPhase = STARTUP_CLOSE_EYES;

// Current servo positions
int currentH = HorizontalLimits::CENTER;
int currentV = VerticalLimits::CENTER;

// Blink state
bool isBlinking = false;
unsigned long blinkStartTime = 0;

// Idle state
unsigned long lastActivityTime = 0;
unsigned long nextIdleBlinkTime = 0;
unsigned long nextIdleSequenceTime = 0;
int currentIdleSequence = 0;
int idleTargetH = HorizontalLimits::CENTER;
int idleTargetV = VerticalLimits::CENTER;

// Startup animation state
unsigned long startupPhaseStartTime = 0;
float eyelidAnimationProgress = 0.0;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

int mapJoystick(int joyValue, int joyMin, int joyMax, int servoMin, int servoMax, bool inverted = false);
int smooth(int current, int target, int smoothing);
int getEyelidPosition(int openValue, int closedValue, bool inverted, bool shouldClose);
void moveEyes(int horizontal, int vertical);
void setEyelidPosition(float position);
void openEyes();
void closeEyes();
void startBlink();
void updateBlink();
void printDebug(int joyX, int joyY, int targetH, int targetV);
void runStartupAnimation();
void runIdleAnimation();
void checkForActivity();
void enterIdleMode();
void exitIdleMode();
int getIdleTarget(int min, int max, int center);

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\n\n========================================"));
  Serial.println(F("Animatronic Eyes - Enhanced System"));
  Serial.println(F("With Startup & Idle Animations"));
  Serial.println(F("========================================\n"));
  
  // Display configuration loaded
  Serial.println(F("Configuration loaded from EyeConfig.h"));
  Serial.print(F("Servo shield address: 0x"));
  Serial.println(SERVO_SHIELD_ADDRESS, HEX);
  Serial.print(F("Idle timeout: "));
  Serial.print(IdleSettings::IDLE_TIMEOUT_MS / 1000);
  Serial.println(F(" seconds"));
  Serial.println();
  
  // Initialize servo shield
  Serial.print(F("Initializing servo shield... "));
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);
  
  Wire.beginTransmission(SERVO_SHIELD_ADDRESS);
  if (Wire.endTransmission() != 0) {
    Serial.println(F("FAILED!"));
    Serial.println(F("Check servo shield connections and address"));
    while(1);
  }
  Serial.println(F("OK"));
  
  // Initialize nunchuck
  Serial.print(F("Initializing nunchuck... "));
  nunchuck.begin();
  int timeout = 0;
  while (nunchuck.type != NUNCHUCK && timeout < 50) {
    nunchuck.readData();
    delay(100);
    timeout++;
  }
  
  if (timeout >= 50) {
    Serial.println(F("FAILED!"));
    Serial.println(F("⚠️  Nunchuck not found - idle mode only"));
  } else {
    Serial.println(F("OK"));
  }
  
  // SAFETY CHECK: Verify limits are sane
  if (HorizontalLimits::MIN >= HorizontalLimits::MAX ||
      VerticalLimits::MIN >= VerticalLimits::MAX) {
    Serial.println(F("\n⚠️  ERROR: Invalid servo limits in EyeConfig.h!"));
    Serial.println(F("MIN must be less than MAX"));
    Serial.println(F("Check your configuration file!"));
    while(1);
  }
  
  Serial.println(F("\n========================================"));
  Serial.println(F("Starting Startup Animation..."));
  Serial.println(F("========================================\n"));
  
  // Initialize startup animation
  currentState = STATE_STARTUP;
  startupPhase = STARTUP_CLOSE_EYES;
  startupPhaseStartTime = millis();
  
  // Seed random number generator
  randomSeed(analogRead(A0));
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // Update nunchuck
  nunchuck.readData();
  
  // ALWAYS check for activity first (before state processing)
  if (currentState == STATE_ACTIVE || currentState == STATE_IDLE) {
    checkForActivity();
  }
  
  // State machine
  switch (currentState) {
    case STATE_STARTUP:
      runStartupAnimation();
      break;
      
    case STATE_ACTIVE:
      {
        // Get joystick values
        int joyX_raw = nunchuck.getJoyX();
        int joyY_raw = nunchuck.getJoyY();
        
        // Map to servo positions using config values WITH inversion support
        int targetH = mapJoystick(joyX_raw, 
                                  NunchuckCalibration::JOY_X_MIN, 
                                  NunchuckCalibration::JOY_X_MAX, 
                                  HorizontalLimits::MIN, 
                                  HorizontalLimits::MAX,
                                  HorizontalLimits::INVERTED);
        int targetV = mapJoystick(joyY_raw, 
                                  NunchuckCalibration::JOY_Y_MIN, 
                                  NunchuckCalibration::JOY_Y_MAX, 
                                  VerticalLimits::MIN, 
                                  VerticalLimits::MAX,
                                  VerticalLimits::INVERTED);
        
        // Smooth movement using config smoothing value
        currentH = smooth(currentH, targetH, MotionSettings::EYE_SMOOTHING);
        currentV = smooth(currentV, targetV, MotionSettings::EYE_SMOOTHING);
        
        // Update eye position
        moveEyes(currentH, currentV);
        
        // Handle Z button - Blink
        static bool lastZ = false;
        if (nunchuck.getButtonZ() && !lastZ && !isBlinking) {
          startBlink();
        }
        lastZ = nunchuck.getButtonZ();
        
        // Handle C button - Return to center
        static bool lastC = false;
        if (nunchuck.getButtonC() && !lastC) {
          Serial.println(F("Returning to center..."));
          currentH = HorizontalLimits::CENTER;
          currentV = VerticalLimits::CENTER;
        }
        lastC = nunchuck.getButtonC();
        
        // Update blink animation
        updateBlink();
        
        // Check if should enter idle mode
        if (millis() - lastActivityTime > IdleSettings::IDLE_TIMEOUT_MS) {
          enterIdleMode();
        }
        
        // Debug output
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 500) {
          lastPrint = millis();
          printDebug(joyX_raw, joyY_raw, targetH, targetV);
        }
      }
      break;
      
    case STATE_IDLE:
      // Run idle animation
      runIdleAnimation();
      
      // Update blink animation
      updateBlink();
      
      // Debug output
      static unsigned long lastIdlePrint = 0;
      if (millis() - lastIdlePrint > 1000) {
        lastIdlePrint = millis();
        Serial.print(F("IDLE | Sequence: "));
        Serial.print(currentIdleSequence);
        Serial.print(F(" | Target H:"));
        Serial.print(idleTargetH);
        Serial.print(F(" V:"));
        Serial.print(idleTargetV);
        Serial.print(F(" | Current H:"));
        Serial.print(currentH);
        Serial.print(F(" V:"));
        Serial.println(currentV);
      }
      break;
  }
  
  delay(SafetyLimits::MIN_UPDATE_INTERVAL_MS);
}

// ============================================================================
// STARTUP ANIMATION
// ============================================================================

void runStartupAnimation() {
  unsigned long elapsed = millis() - startupPhaseStartTime;
  
  switch (startupPhase) {
    case STARTUP_CLOSE_EYES:
      // Smoothly close eyes
      eyelidAnimationProgress += 0.05;
      if (eyelidAnimationProgress >= 1.0) {
        eyelidAnimationProgress = 1.0;
        startupPhase = STARTUP_HOLD_CLOSED;
        startupPhaseStartTime = millis();
        Serial.println(F("Eyes closed..."));
      }
      setEyelidPosition(eyelidAnimationProgress);  // 1.0 = closed
      break;
      
    case STARTUP_HOLD_CLOSED:
      // Hold eyes closed
      if (elapsed >= StartupSettings::EYES_CLOSED_HOLD) {
        startupPhase = STARTUP_OPEN_EYES;
        startupPhaseStartTime = millis();
        eyelidAnimationProgress = 1.0;
        Serial.println(F("Opening eyes..."));
      }
      break;
      
    case STARTUP_OPEN_EYES:
      // Slowly open eyes
      eyelidAnimationProgress -= 0.02;
      if (eyelidAnimationProgress <= 0.0) {
        eyelidAnimationProgress = 0.0;
        startupPhase = STARTUP_LOOK_AROUND;
        startupPhaseStartTime = millis();
        Serial.println(F("Looking around..."));
      }
      setEyelidPosition(eyelidAnimationProgress);  // 0.0 = open
      break;
      
    case STARTUP_LOOK_AROUND:
      // Look left, then right, then up, then down
      if (elapsed < 500) {
        // Look left
        currentH = smooth(currentH, HorizontalLimits::MIN, 5);
      } else if (elapsed < 1000) {
        // Look right
        currentH = smooth(currentH, HorizontalLimits::MAX, 5);
      } else if (elapsed < 1500) {
        // Look up
        currentV = smooth(currentV, VerticalLimits::MAX, 5);
      } else if (elapsed < 2000) {
        // Look down
        currentV = smooth(currentV, VerticalLimits::MIN, 5);
      } else {
        startupPhase = STARTUP_CENTER;
        startupPhaseStartTime = millis();
        Serial.println(F("Centering..."));
      }
      moveEyes(currentH, currentV);
      break;
      
    case STARTUP_CENTER:
      // Return to center
      currentH = smooth(currentH, HorizontalLimits::CENTER, 8);
      currentV = smooth(currentV, VerticalLimits::CENTER, 8);
      moveEyes(currentH, currentV);
      
      if (elapsed >= StartupSettings::RETURN_TO_CENTER) {
        startupPhase = STARTUP_COMPLETE;
        Serial.println(F("\n========================================"));
        Serial.println(F("Startup Complete!"));
        Serial.println(F("========================================"));
        Serial.println(F("Controls:"));
        Serial.println(F("  Joystick → Move eyes"));
        Serial.println(F("  Z Button → Blink"));
        Serial.println(F("  C Button → Return to center"));
        Serial.print(F("  Idle "));
        Serial.print(IdleSettings::IDLE_TIMEOUT_MS / 1000);
        Serial.println(F(" sec → Auto animation"));
        Serial.println(F("========================================\n"));
        
        // Transition to active state
        currentState = STATE_ACTIVE;
        lastActivityTime = millis();
      }
      break;
      
    case STARTUP_COMPLETE:
      // Should never reach here
      break;
  }
}

// ============================================================================
// IDLE ANIMATION
// ============================================================================

void runIdleAnimation() {
  unsigned long currentTime = millis();
  
  // Auto-blink
  if (currentTime >= nextIdleBlinkTime && !isBlinking) {
    startBlink();
    nextIdleBlinkTime = currentTime + random(IdleSettings::IDLE_BLINK_MIN, 
                                              IdleSettings::IDLE_BLINK_MAX);
  }
  
  // Movement sequences
  if (currentTime >= nextIdleSequenceTime) {
    // Pick a new random sequence
    currentIdleSequence = random(0, 5);
    
    // Generate new target positions based on sequence
    switch (currentIdleSequence) {
      case 0:  // Look left
        idleTargetH = getIdleTarget(HorizontalLimits::MIN, HorizontalLimits::CENTER, HorizontalLimits::CENTER);
        idleTargetV = VerticalLimits::CENTER;  // FIXED: was HorizontalLimits::CENTER
        break;
        
      case 1:  // Look right
        idleTargetH = getIdleTarget(HorizontalLimits::CENTER, HorizontalLimits::MAX, HorizontalLimits::CENTER);
        idleTargetV = VerticalLimits::CENTER;
        break;
        
      case 2:  // Look up
        idleTargetH = HorizontalLimits::CENTER;
        idleTargetV = getIdleTarget(VerticalLimits::CENTER, VerticalLimits::MAX, VerticalLimits::CENTER);
        break;
        
      case 3:  // Look down
        idleTargetH = HorizontalLimits::CENTER;
        idleTargetV = getIdleTarget(VerticalLimits::MIN, VerticalLimits::CENTER, VerticalLimits::CENTER);
        break;
        
      case 4:  // Look diagonal (random corner)
        idleTargetH = random(0, 2) ? 
          getIdleTarget(HorizontalLimits::MIN, HorizontalLimits::CENTER, HorizontalLimits::CENTER) :
          getIdleTarget(HorizontalLimits::CENTER, HorizontalLimits::MAX, HorizontalLimits::CENTER);
        idleTargetV = random(0, 2) ?
          getIdleTarget(VerticalLimits::MIN, VerticalLimits::CENTER, VerticalLimits::CENTER) :
          getIdleTarget(VerticalLimits::CENTER, VerticalLimits::MAX, VerticalLimits::CENTER);
        break;
    }
    
    // Schedule next sequence
    nextIdleSequenceTime = currentTime + random(IdleSettings::IDLE_SEQUENCE_MIN, 
                                                 IdleSettings::IDLE_SEQUENCE_MAX);
  }
  
  // Smoothly move toward idle target
  currentH = smooth(currentH, idleTargetH, MotionSettings::EYE_SMOOTHING);
  currentV = smooth(currentV, idleTargetV, MotionSettings::EYE_SMOOTHING);
  moveEyes(currentH, currentV);
}

// ============================================================================
// IDLE MODE MANAGEMENT
// ============================================================================

void enterIdleMode() {
  Serial.println(F("\n>>> ENTERING IDLE MODE <<<"));
  Serial.println(F("Eyes will look around automatically"));
  Serial.println(F("Move joystick or press button to exit\n"));
  
  currentState = STATE_IDLE;
  
  // Initialize idle timers
  unsigned long currentTime = millis();
  nextIdleBlinkTime = currentTime + random(IdleSettings::IDLE_BLINK_MIN, 
                                            IdleSettings::IDLE_BLINK_MAX);
  nextIdleSequenceTime = currentTime + 500;  // Start first sequence after 500ms
  currentIdleSequence = 0;
  
  // Set initial idle targets
  idleTargetH = HorizontalLimits::CENTER;
  idleTargetV = VerticalLimits::CENTER;
  
  Serial.print(F("Next blink in: "));
  Serial.print((nextIdleBlinkTime - currentTime) / 1000);
  Serial.println(F(" seconds"));
  Serial.println(F("Starting idle sequences...\n"));
}

void exitIdleMode() {
  if (currentState == STATE_IDLE) {
    Serial.println(F("\n>>> EXITING IDLE MODE <<<\n"));
    currentState = STATE_ACTIVE;
    lastActivityTime = millis();
  }
}

void checkForActivity() {
  // Read nunchuck
  int joyX = nunchuck.getJoyX();
  int joyY = nunchuck.getJoyY();
  int joyCenter = NunchuckCalibration::CENTER;
  
  // Check if joystick moved
  if (abs(joyX - joyCenter) > DEADZONE || abs(joyY - joyCenter) > DEADZONE) {
    if (currentState == STATE_IDLE) {
      exitIdleMode();
    }
    lastActivityTime = millis();
  }
  
  // Check if any button pressed
  if (nunchuck.getButtonZ() || nunchuck.getButtonC()) {
    if (currentState == STATE_IDLE) {
      exitIdleMode();
    }
    lastActivityTime = millis();
  }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Get idle target position within a range
 * Uses IDLE_MOVEMENT_RANGE to limit how far eyes wander
 */
int getIdleTarget(int min, int max, int center) {
  int range = max - min;
  int limitedRange = range * IdleSettings::IDLE_MOVEMENT_RANGE;
  int limitedMin = center - (limitedRange / 2);
  int limitedMax = center + (limitedRange / 2);
  return random(limitedMin, limitedMax);
}

/**
 * Set eyelid position (0.0 = open, 1.0 = closed)
 * Used for smooth animations
 */
void setEyelidPosition(float position) {
  // Clamp position
  position = constrain(position, 0.0, 1.0);
  
  // Calculate pulse values for each eyelid
  int luPos = LeftUpperLid::OPEN + (LeftUpperLid::CLOSED - LeftUpperLid::OPEN) * position;
  int llPos = LeftLowerLid::OPEN + (LeftLowerLid::CLOSED - LeftLowerLid::OPEN) * position;
  int ruPos = RightUpperLid::OPEN + (RightUpperLid::CLOSED - RightUpperLid::OPEN) * position;
  int rlPos = RightLowerLid::OPEN + (RightLowerLid::CLOSED - RightLowerLid::OPEN) * position;
  
  // Apply inversion
  if (LeftUpperLid::INVERTED) luPos = LeftUpperLid::CLOSED - (luPos - LeftUpperLid::OPEN);
  if (LeftLowerLid::INVERTED) llPos = LeftLowerLid::CLOSED - (llPos - LeftLowerLid::OPEN);
  if (RightUpperLid::INVERTED) ruPos = RightUpperLid::CLOSED - (ruPos - RightUpperLid::OPEN);
  if (RightLowerLid::INVERTED) rlPos = RightLowerLid::CLOSED - (rlPos - RightLowerLid::OPEN);
  
  // Set servos
  pwm.setPWM(SERVO_L_UPPER, 0, luPos);
  pwm.setPWM(SERVO_L_LOWER, 0, llPos);
  pwm.setPWM(SERVO_R_UPPER, 0, ruPos);
  pwm.setPWM(SERVO_R_LOWER, 0, rlPos);
}

/**
 * Map joystick value to servo position with deadzone and inversion support
 */
int mapJoystick(int joyValue, int joyMin, int joyMax, int servoMin, int servoMax, bool inverted) {
  // Apply deadzone around center
  int joyCenter = (joyMin + joyMax) / 2;
  if (abs(joyValue - joyCenter) < DEADZONE) {
    // In deadzone - return servo center
    return (servoMin + servoMax) / 2;
  }
  
  // Map joystick range to servo range
  int servoPos;
  if (inverted) {
    // Inverted: reverse the mapping
    servoPos = map(joyValue, joyMin, joyMax, servoMax, servoMin);
  } else {
    // Normal: standard mapping
    servoPos = map(joyValue, joyMin, joyMax, servoMin, servoMax);
  }
  
  return constrain(servoPos, min(servoMin, servoMax), max(servoMin, servoMax));
}

/**
 * Simple smoothing function
 */
int smooth(int current, int target, int smoothing) {
  int delta = target - current;
  return current + (delta / smoothing);
}

/**
 * Get eyelid position with inversion support
 */
int getEyelidPosition(int openValue, int closedValue, bool inverted, bool shouldClose) {
  if (inverted) {
    // Inverted: swap open and closed
    return shouldClose ? openValue : closedValue;
  } else {
    // Normal orientation
    return shouldClose ? closedValue : openValue;
  }
}

/**
 * Move eyes to position
 */
void moveEyes(int horizontal, int vertical) {
  // Safety: constrain to configured limits
  horizontal = constrain(horizontal, HorizontalLimits::MIN, HorizontalLimits::MAX);
  vertical = constrain(vertical, VerticalLimits::MIN, VerticalLimits::MAX);
  
  pwm.setPWM(SERVO_HORIZONTAL, 0, horizontal);
  pwm.setPWM(SERVO_VERTICAL, 0, vertical);
}

/**
 * Open eyes (with inversion support)
 */
void openEyes() {
  pwm.setPWM(SERVO_L_UPPER, 0, 
    getEyelidPosition(LeftUpperLid::OPEN, LeftUpperLid::CLOSED, LeftUpperLid::INVERTED, false));
  pwm.setPWM(SERVO_L_LOWER, 0, 
    getEyelidPosition(LeftLowerLid::OPEN, LeftLowerLid::CLOSED, LeftLowerLid::INVERTED, false));
  pwm.setPWM(SERVO_R_UPPER, 0, 
    getEyelidPosition(RightUpperLid::OPEN, RightUpperLid::CLOSED, RightUpperLid::INVERTED, false));
  pwm.setPWM(SERVO_R_LOWER, 0, 
    getEyelidPosition(RightLowerLid::OPEN, RightLowerLid::CLOSED, RightLowerLid::INVERTED, false));
}

/**
 * Close eyes (with inversion support)
 */
void closeEyes() {
  pwm.setPWM(SERVO_L_UPPER, 0, 
    getEyelidPosition(LeftUpperLid::OPEN, LeftUpperLid::CLOSED, LeftUpperLid::INVERTED, true));
  pwm.setPWM(SERVO_L_LOWER, 0, 
    getEyelidPosition(LeftLowerLid::OPEN, LeftLowerLid::CLOSED, LeftLowerLid::INVERTED, true));
  pwm.setPWM(SERVO_R_UPPER, 0, 
    getEyelidPosition(RightUpperLid::OPEN, RightUpperLid::CLOSED, RightUpperLid::INVERTED, true));
  pwm.setPWM(SERVO_R_LOWER, 0, 
    getEyelidPosition(RightLowerLid::OPEN, RightLowerLid::CLOSED, RightLowerLid::INVERTED, true));
}

/**
 * Start blink animation
 */
void startBlink() {
  isBlinking = true;
  blinkStartTime = millis();
  closeEyes();
}

/**
 * Update blink animation
 */
void updateBlink() {
  if (isBlinking) {
    unsigned long elapsed = millis() - blinkStartTime;
    if (elapsed >= BLINK_DURATION) {
      openEyes();
      isBlinking = false;
    }
  }
}

/**
 * Print debug info
 */
void printDebug(int joyX, int joyY, int targetH, int targetV) {
  Serial.print(F("State: "));
  switch (currentState) {
    case STATE_STARTUP: Serial.print(F("STARTUP")); break;
    case STATE_ACTIVE: Serial.print(F("ACTIVE")); break;
    case STATE_IDLE: Serial.print(F("IDLE")); break;
  }
  
  Serial.print(F(" | Joy X:"));
  Serial.print(joyX);
  Serial.print(F(" Y:"));
  Serial.print(joyY);
  Serial.print(F(" | Eye H:"));
  Serial.print(currentH);
  Serial.print(F(" V:"));
  Serial.print(currentV);
  
  if (currentState == STATE_ACTIVE) {
    unsigned long timeUntilIdle = IdleSettings::IDLE_TIMEOUT_MS - (millis() - lastActivityTime);
    if (timeUntilIdle < IdleSettings::IDLE_TIMEOUT_MS) {
      Serial.print(F(" | Idle in: "));
      Serial.print(timeUntilIdle / 1000);
      Serial.print(F("s"));
    }
  }
  
  Serial.println();
}
