/**
 * EyeConfig.h
 * 
 * Configuration file for Animatronic Eyes Integration Test
 * 
 * IMPORTANT: These values define SAFE operating limits for your mechanism
 * DO NOT change these without recalibrating servos!
 * Incorrect values can damage your mechanical linkages!
 * 
 * WHEN TO UPDATE THIS FILE:
 * - Initial calibration (set all values)
 * - Servo replacement (recalibrate that channel)
 * - Mechanical changes (reassembly, new linkages)
 * - If servo struggles or buzzes (tighten limits)
 * 
 * CALIBRATION DATE: December 2024
 * MECHANISM: Simplified 3D Printed Dual Eye Mechanism (Instructables)
 * SERVOS: SG90 (Amazon - https://www.amazon.com/dp/B0B68NTZ6H)
 */

#ifndef EYE_CONFIG_H
#define EYE_CONFIG_H

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

/**
 * Servo Shield I2C Address
 * Default: 0x40
 * If A0 bridged: 0x41
 * If A1 bridged: 0x42
 * If A2 bridged: 0x44  ← YOUR CONFIGURATION
 * If A3 bridged: 0x48
 * If A4 bridged: 0x50
 * If A5 bridged: 0x60
 */
#define SERVO_SHIELD_ADDRESS 0x44  // A2 bridged

/**
 * Servo Channel Assignments
 * These define which servo is connected to which channel on the shield
 */
#define SERVO_HORIZONTAL  0  // Left/Right eye movement
#define SERVO_VERTICAL    1  // Up/Down eye movement
#define SERVO_L_UPPER     2  // Left eye upper eyelid
#define SERVO_L_LOWER     3  // Left eye lower eyelid
#define SERVO_R_UPPER     4  // Right eye upper eyelid
#define SERVO_R_LOWER     5  // Right eye lower eyelid

// ============================================================================
// CALIBRATED SERVO LIMITS - YOUR VALUES
// ============================================================================

/**
 * Horizontal Eye Movement (Channel 0)
 * 
 * CALIBRATION DATA:
 * - Tested: December 2024
 * - MIN: 220 (leftmost safe position)
 * - MAX: 470 (rightmost safe position)
 * - CENTER: 345 (eyes looking straight ahead)
 * - Range: 250 pulse units
 * 
 * INVERSION:
 * - Set INVERTED = true if joystick left/right is backwards
 * - When true: Joystick left → eyes right, joystick right → eyes left
 * - When false: Joystick left → eyes left (normal)
 * 
 * SAFETY NOTES:
 * - DO NOT decrease MIN below 220 (mechanical binding)
 * - DO NOT increase MAX above 470 (mechanical binding)
 * - If servo buzzes, reduce range immediately
 */
struct HorizontalLimits {
  static const int MIN = 220;
  static const int CENTER = 345;
  static const int MAX = 470;
  static const bool INVERTED = true;  // Set true to reverse left/right
};

/**
 * Vertical Eye Movement (Channel 1)
 * 
 * CALIBRATION DATA:
 * - Tested: December 2024
 * - MIN: 285 (downward safe position)
 * - MAX: 400 (upward safe position)
 * - CENTER: 342 (eyes looking level)
 * - Range: 115 pulse units
 * 
 * INVERSION:
 * - Set INVERTED = true if joystick up/down is backwards
 * - When true: Joystick up → eyes down, joystick down → eyes up
 * - When false: Joystick up → eyes up (normal)
 * 
 * SAFETY NOTES:
 * - Vertical range is SMALLER than horizontal (normal for eye mechanisms)
 * - DO NOT decrease MIN below 285
 * - DO NOT increase MAX above 400
 */
struct VerticalLimits {
  static const int MIN = 260;
  static const int CENTER = 342;
  static const int MAX = 440;
  static const bool INVERTED = false;  // Set true to reverse up/down
};

/**
 * Left Eye Upper Eyelid (Channel 2)
 * 
 * CALIBRATION DATA:
 * - Tested: December 2024
 * - OPEN: 130 (eyelid fully retracted, pupil visible)
 * - CLOSED: 610 (eyelid extended, covers pupil)
 * - HALF: 370 (half-closed for expressions)
 * - Range: 480 pulse units
 * 
 * INVERSION:
 * - Set to true if servo motion is backwards
 * - true = swap OPEN and CLOSED values
 * - false = use values as-is (normal)
 * 
 * SAFETY NOTES:
 * - OPEN position should NOT block pupil
 * - CLOSED position should meet/overlap lower lid
 * - If eyelid binds, adjust limits
 */
struct LeftUpperLid {
  static const int OPEN = 300; //260;      // YOUR calibrated value
  static const int CLOSED = 410; //410;    // YOUR calibrated value
  static const int HALF = 335;      // Calculated: (260+410)/2
  static const bool INVERTED = false;  // YOUR setting
};

/**
 * Left Eye Lower Eyelid (Channel 3)
 * 
 * CALIBRATION DATA:
 * - Tested: December 2024
 * - OPEN: 135 (eyelid fully retracted)
 * - CLOSED: 615 (eyelid extended, meets upper lid)
 * - HALF: 375 (half-closed)
 * - Range: 480 pulse units
 */
struct LeftLowerLid {
  static const int OPEN = 280;      // YOUR calibrated value
  static const int CLOSED = 400;    // YOUR calibrated value
  static const int HALF = 340;      // Calculated: (280+400)/2
  static const bool INVERTED = true;  // YOUR setting - INVERTED!
};

/**
 * Right Eye Upper Eyelid (Channel 4)
 * 
 * CALIBRATION DATA:
 * - Tested: December 2024
 * - OPEN: 115 (eyelid fully retracted)
 * - CLOSED: 595 (eyelid extended, covers pupil)
 * - HALF: 355 (half-closed)
 * - Range: 480 pulse units
 * 
 * SAFETY NOTES:
 * - Right eyelid values differ from left (normal - servo variation)
 * - Each eyelid calibrated independently
 */
struct RightUpperLid {
  static const int OPEN = 255; //255;      // YOUR calibrated value
  static const int CLOSED = 380; //425;    // YOUR calibrated value
  static const int HALF = 340;      // Calculated: (255+425)/2
  static const bool INVERTED = true;  // YOUR setting - INVERTED!
};

/**
 * Right Eye Lower Eyelid (Channel 5)
 * 
 * CALIBRATION DATA:
 * - Tested: December 2024
 * - OPEN: 105 (eyelid fully retracted)
 * - CLOSED: 585 (eyelid extended, meets upper lid)
 * - HALF: 345 (half-closed)
 * - Range: 480 pulse units
 */
struct RightLowerLid {
  static const int OPEN = 280;      // YOUR calibrated value
  static const int CLOSED = 395;    // YOUR calibrated value
  static const int HALF = 325;      // Calculated: (255+395)/2
  static const bool INVERTED = false;  // YOUR setting
};

// ============================================================================
// NUNCHUCK CALIBRATION
// ============================================================================

/**
 * Nunchuck Joystick Calibration
 * 
 * CALIBRATION DATA:
 * - Tested: December 2024
 * - JoyX MIN: 26 (full left)
 * - JoyX MAX: 226 (full right)
 * - JoyY MIN: 26 (full down) [assumed same as X, verify if needed]
 * - JoyY MAX: 226 (full up) [assumed same as X, verify if needed]
 * - Center: 126 (calculated from min/max)
 * - Range: 200 pulse units
 * 
 * NOTES:
 * - Your nunchuck has slightly asymmetric range (not perfectly 0-255)
 * - This is normal for Wii nunchucks
 * - Deadzone compensates for center drift
 */
struct NunchuckCalibration {
  static const int JOY_X_MIN = 26;
  static const int JOY_X_MAX = 226;
  static const int JOY_Y_MIN = 26;
  static const int JOY_Y_MAX = 226;
  static const int CENTER = 126;  // (26 + 226) / 2
};

// ============================================================================
// MOTION CONTROL PARAMETERS
// ============================================================================

/**
 * Motion Smoothing
 * 
 * Controls how smoothly servos move to target positions
 * Higher value = smoother, slower movement
 * Lower value = faster, more responsive movement
 * 
 * Recommended ranges:
 * - Eye movement: 6-12 (Disney typically uses 8-10)
 * - Eyelids: 3-6 (faster for snappy blinks)
 */
struct MotionSettings {
  static const int EYE_SMOOTHING = 8;      // Smoothing for eye movement
  static const int EYELID_SMOOTHING = 5;   // Smoothing for eyelids (not used in basic test)
};

/**
 * Joystick Deadzone
 * 
 * Size of the "dead zone" around joystick center
 * Prevents jitter when joystick is released
 * 
 * If eyes shake when joystick centered: INCREASE this value
 * If hard to fine-control eyes: DECREASE this value
 * 
 * Recommended: 8-15
 */
#define DEADZONE 10

/**
 * Blink Duration
 * 
 * How long eyes stay closed during blink (milliseconds)
 * 
 * Natural human blink: 100-150ms
 * Cartoon blink: 200-300ms
 * Surprised blink: 50-100ms
 */
#define BLINK_DURATION 150  // milliseconds

/**
 * Idle Animation Settings
 * 
 * Controls automatic behavior when nunchuck is not being used
 * Makes eyes "come alive" with autonomous movement
 */
struct IdleSettings {
  // How long to wait before starting idle animation (milliseconds)
  static const unsigned long IDLE_TIMEOUT_MS = 15000;  // 15 seconds
  
  // Auto-blink timing during idle (milliseconds)
  static const unsigned long IDLE_BLINK_MIN = 2000;    // Minimum 2 seconds between blinks
  static const unsigned long IDLE_BLINK_MAX = 6000;    // Maximum 6 seconds between blinks
  
  // Movement sequence timing
  static const unsigned long IDLE_SEQUENCE_MIN = 2000;  // Min time for each look sequence
  static const unsigned long IDLE_SEQUENCE_MAX = 4000;  // Max time for each look sequence
  
  // Movement ranges (percentage of full range, 0.0 - 1.0)
  static constexpr float IDLE_MOVEMENT_RANGE = 0.7;    // Use 70% of available range
};

/**
 * Startup Animation Settings
 * 
 * Defines the boot sequence when system powers on
 */
struct StartupSettings {
  static const unsigned long EYES_CLOSED_HOLD = 1000;   // Hold eyes closed for 1 second
  static const unsigned long EYES_OPEN_DURATION = 800;  // Take 800ms to open eyes
  static const unsigned long LOOK_AROUND_DURATION = 2000; // Look around for 2 seconds
  static const unsigned long RETURN_TO_CENTER = 500;    // Return to center in 500ms
};

// ============================================================================
// SAFETY LIMITS
// ============================================================================

/**
 * Absolute Safety Constraints
 * 
 * These are hard limits that should NEVER be exceeded
 * They protect servos and mechanism from damage
 */
struct SafetyLimits {
  // Servo pulse width absolute limits
  static const int ABSOLUTE_MIN_PULSE = 100;   // Below this = danger
  static const int ABSOLUTE_MAX_PULSE = 650;   // Above this = danger
  
  // Maximum position change per update (prevents sudden jerks)
  static const int MAX_DELTA_PER_UPDATE = 50;
  
  // Update rate limit (minimum ms between servo updates)
  static const int MIN_UPDATE_INTERVAL_MS = 20;  // 50Hz max
};

// ============================================================================
// SERVO REPLACEMENT GUIDE
// ============================================================================

/**
 * WHEN YOU NEED TO REPLACE A SERVO:
 * 
 * 1. Note which channel the failed servo is on (0-5)
 * 
 * 2. Install new servo in same position
 * 
 * 3. Upload ServoPulseCalibrator.ino
 * 
 * 4. Set SERVO_CHANNEL to match the replaced servo
 * 
 * 5. Find new MIN and MAX values for that servo
 * 
 * 6. Update the corresponding struct in this file:
 *    - Channel 0 → HorizontalLimits
 *    - Channel 1 → VerticalLimits
 *    - Channel 2 → LeftUpperLid
 *    - Channel 3 → LeftLowerLid
 *    - Channel 4 → RightUpperLid
 *    - Channel 5 → RightLowerLid
 * 
 * 7. Recalculate CENTER (for eyes) or HALF (for eyelids)
 * 
 * 8. Save this file with new calibration date
 * 
 * 9. Re-upload EyesIntegrationTest.ino
 * 
 * 10. Test carefully before full operation
 */

// ============================================================================
// CALIBRATION HISTORY LOG
// ============================================================================

/**
 * CALIBRATION HISTORY:
 * 
 * [December 2024] - Initial Calibration
 * - All 6 servos calibrated
 * - Mechanism: Simplified 3D Printed Dual Eye
 * - Servos: SG90 from Amazon
 * - Tested without mechanical load
 * - Nunchuck: JoyX range 26-226
 * 
 * [Add your updates here]
 * [Date] - Channel X replaced
 * - Old values: MIN=___, MAX=___
 * - New values: MIN=___, MAX=___
 * - Reason: Servo failure / mechanical change
 */

#endif // EYE_CONFIG_H
