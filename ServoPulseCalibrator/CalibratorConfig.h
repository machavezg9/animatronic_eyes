/**
 * CalibratorConfig.h
 * 
 * Configuration file for Servo Pulse Width Calibrator
 * 
 * CRITICAL SAFETY:
 * This file defines ABSOLUTE SAFE limits that the calibrator will not exceed
 * These protect your mechanism from damage during calibration
 * 
 * WHEN TO UPDATE THIS FILE:
 * - Before first calibration (set conservative limits)
 * - When replacing a servo (set limits for that channel)
 * - After mechanical changes (adjust to new constraints)
 * - If you know safe operating range (narrow the window)
 * 
 * HOW TO USE:
 * 1. Set SERVO_CHANNEL to the channel you're testing
 * 2. Set SERVO_NAME for identification
 * 3. Set ABSOLUTE_MIN/MAX to protect mechanism
 * 4. Set STARTING_MIN/MAX to reasonable initial values
 * 5. Upload and calibrate within these boundaries
 */

#ifndef CALIBRATOR_CONFIG_H
#define CALIBRATOR_CONFIG_H

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

/**
 * Servo Shield I2C Address
 * Change this to match your hardware setup
 */
#define SERVO_SHIELD_ADDRESS 0x44  // A2 bridged (YOUR configuration)

// ============================================================================
// SERVO BEING CALIBRATED
// ============================================================================

/**
 * Which servo channel are you calibrating?
 * Change this for each servo you test
 * 
 * Channel 0: Horizontal Eye Movement
 * Channel 1: Vertical Eye Movement
 * Channel 2: Left Eye Upper Eyelid
 * Channel 3: Left Eye Lower Eyelid
 * Channel 4: Right Eye Upper Eyelid
 * Channel 5: Right Eye Lower Eyelid
 */
#define SERVO_CHANNEL 5

/**
 * Servo name for display (helps you keep track)
 * Change this to match the servo you're testing
 */
const char* SERVO_NAME = "Horizontal Eye (Channel 0)";

// ============================================================================
// SAFETY LIMITS - CRITICAL!
// ============================================================================

/**
 * ABSOLUTE SAFETY BOUNDARIES
 * 
 * The calibrator will NEVER allow servo to go beyond these limits
 * Set these based on:
 * - Known safe range from previous calibration
 * - Mechanical constraints of your mechanism
 * - Conservative estimates if unsure
 * 
 * IMPORTANT: These are HARD LIMITS - calibrator cannot exceed them!
 * 
 * If you have NO mechanical load (servo free-running):
 * - Can use wider range: 100-650
 * 
 * If you have mechanism attached:
 * - Use NARROW range based on known safe values
 * - Better to be conservative!
 */

// Default conservative limits for SG90 servos
// #define ABSOLUTE_MIN_PULSE 100   // Never go below this (HARD FLOOR)
// #define ABSOLUTE_MAX_PULSE 650   // Never go above this (HARD CEILING)

/**
 * STARTING VALUES
 * 
 * Where the servo starts when calibrator begins
 * Should be in the middle of your expected range
 * 
 * Recommendations:
 * - First time calibrating: 150 and 600 (safe, wide range)
 * - Re-calibrating known servo: Use your previous MIN/MAX
 * - Servo attached to mechanism: Use very conservative values!
 */
// #define STARTING_MIN_PULSE 150   // Initial MIN position
// #define STARTING_MAX_PULSE 600   // Initial MAX position

/**
 * DEFAULT STEP SIZE
 * 
 * How many pulse units to change per button press
 * 
 * Recommendations:
 * - Finding range: 10-20 (fast exploration)
 * - Fine-tuning: 1-5 (precision)
 * - Default: 5 (good balance)
 */
#define DEFAULT_STEP_SIZE 5

// ============================================================================
// PRESET CONFIGURATIONS FOR EACH CHANNEL
// ============================================================================

/**
 * PRESET SAFE RANGES
 * 
 * If you've already calibrated servos before, you can use these
 * presets to set tighter safety boundaries
 * 
 * Uncomment ONE of these sections to use preset values
 * (Leave all commented to use defaults above)
 */

// ---- CHANNEL 0: Horizontal Eye Movement ----
// Based on your previous calibration: 220-470
// #define ABSOLUTE_MIN_PULSE 200   // Buffer below known MIN (220)
// #define ABSOLUTE_MAX_PULSE 490   // Buffer above known MAX (470)
// #define STARTING_MIN_PULSE 220
// #define STARTING_MAX_PULSE 470

// ---- CHANNEL 1: Vertical Eye Movement ----
// Based on your previous calibration: 285-400
// #define ABSOLUTE_MIN_PULSE 240   // Buffer below known MIN
// #define ABSOLUTE_MAX_PULSE 460   // Buffer above known MAX
// #define STARTING_MIN_PULSE 260
// #define STARTING_MAX_PULSE 440

// ---- CHANNEL 2: Left Upper Eyelid ----
// Based on your previous calibration: 130-610
// #define ABSOLUTE_MIN_PULSE 240   // Buffer below known MIN
// #define ABSOLUTE_MAX_PULSE 430   // Buffer above known MAX
// #define STARTING_MIN_PULSE 260
// #define STARTING_MAX_PULSE 410

// // ---- CHANNEL 3: Left Lower Eyelid ----
// // Based on your previous calibration: 135-615
// #define ABSOLUTE_MIN_PULSE 300   // Buffer below known MIN
// #define ABSOLUTE_MAX_PULSE 420   // Buffer above known MAX
// #define STARTING_MIN_PULSE 280
// #define STARTING_MAX_PULSE 400

// // ---- CHANNEL 4: Right Upper Eyelid ----
// // Based on your previous calibration: 115-595
// #define ABSOLUTE_MIN_PULSE 255    // Buffer below known MIN
// #define ABSOLUTE_MAX_PULSE 425   // Buffer above known MAX
// #define STARTING_MIN_PULSE 275
// #define STARTING_MAX_PULSE 405

// ---- CHANNEL 5: Right Lower Eyelid ----
// Based on your previous calibration: 105-585
// #define ABSOLUTE_MIN_PULSE 255    // Buffer below known MIN
// #define ABSOLUTE_MAX_PULSE 395   // Buffer above known MAX
// #define STARTING_MIN_PULSE 275
// #define STARTING_MAX_PULSE 375

// ============================================================================
// SAFETY NOTES
// ============================================================================

/**
 * HOW TO SET SAFETY LIMITS:
 * 
 * SCENARIO 1: First time calibrating (no mechanism attached)
 * - ABSOLUTE_MIN_PULSE = 100 (wide range, safe)
 * - ABSOLUTE_MAX_PULSE = 650
 * - STARTING_MIN_PULSE = 150
 * - STARTING_MAX_PULSE = 600
 * - Find full range, write it down
 * 
 * SCENARIO 2: Re-calibrating known servo (no mechanism)
 * - Use previous calibration +/- 20 for absolute limits
 * - Start at previous calibration values
 * - Example: If previous was 220-470:
 *   - ABSOLUTE: 200-490 (20 unit buffer)
 *   - STARTING: 220-470 (previous values)
 * 
 * SCENARIO 3: Servo attached to mechanism (MOST DANGEROUS)
 * - Use VERY conservative absolute limits
 * - If you know safe range is 220-470:
 *   - ABSOLUTE: 210-480 (small buffer)
 *   - STARTING: 220-470 (known safe)
 * - Use small step size (1-2)
 * - Move VERY slowly
 * - STOP immediately if you hear buzzing
 * 
 * SCENARIO 4: Replacing failed servo on mechanism
 * - Use old servo's values as starting point
 * - Set absolute limits tight (±10 from old values)
 * - New servo might need slightly different values
 * - Example: Old servo was 220-470:
 *   - ABSOLUTE: 210-480
 *   - STARTING: 220-470
 *   - Fine-tune from there
 */

// ============================================================================
// VALIDATION
// ============================================================================

/**
 * Compile-time safety checks
 * These ensure you haven't set invalid values
 */
#if ABSOLUTE_MIN_PULSE >= ABSOLUTE_MAX_PULSE
  #error "ERROR: ABSOLUTE_MIN_PULSE must be less than ABSOLUTE_MAX_PULSE"
#endif

#if STARTING_MIN_PULSE < ABSOLUTE_MIN_PULSE || STARTING_MIN_PULSE > ABSOLUTE_MAX_PULSE
  #error "ERROR: STARTING_MIN_PULSE must be within ABSOLUTE limits"
#endif

#if STARTING_MAX_PULSE < ABSOLUTE_MIN_PULSE || STARTING_MAX_PULSE > ABSOLUTE_MAX_PULSE
  #error "ERROR: STARTING_MAX_PULSE must be within ABSOLUTE limits"
#endif

#if STARTING_MIN_PULSE >= STARTING_MAX_PULSE
  #error "ERROR: STARTING_MIN_PULSE must be less than STARTING_MAX_PULSE"
#endif

#if DEFAULT_STEP_SIZE < 1 || DEFAULT_STEP_SIZE > 50
  #error "ERROR: DEFAULT_STEP_SIZE must be between 1 and 50"
#endif

// ============================================================================
// QUICK REFERENCE GUIDE
// ============================================================================

/**
 * BEFORE YOU CALIBRATE - CHECKLIST:
 * 
 * 1. [ ] Set SERVO_CHANNEL to correct channel (0-5)
 * 2. [ ] Update SERVO_NAME for clarity
 * 3. [ ] Set ABSOLUTE_MIN_PULSE (safety floor)
 * 4. [ ] Set ABSOLUTE_MAX_PULSE (safety ceiling)
 * 5. [ ] Set STARTING_MIN_PULSE (reasonable initial value)
 * 6. [ ] Set STARTING_MAX_PULSE (reasonable initial value)
 * 7. [ ] Consider DEFAULT_STEP_SIZE (1-5 for attached mechanism)
 * 8. [ ] Verify mechanism won't bind in this range
 * 9. [ ] Upload and test CAREFULLY
 * 10. [ ] Write down final calibrated values
 * 
 * REMEMBER: It's better to have a narrower safe range than to
 *           damage your mechanism!
 */

#endif // CALIBRATOR_CONFIG_H
