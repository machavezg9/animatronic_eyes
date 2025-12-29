# Animatronic Eyes (6-Axis Dual Eye Mechanism)

Dual animatronic eye mechanism with **smooth motion interpolation**, **autonomous behaviors** (startup + idle + blink), and a **calibration-first workflow** that prevents servo binding and makes replacements fast.

**Inspired by:** [Will Cogley’s Simplified Eye Mechanism](https://www.instructables.com/Simplified-3D-Printed-Animatronic-Dual-Eye-Mechani/) 
**Status:** Working prototype / ongoing improvements

---

## Demo
- **Portfolio page:** https://miguelchavez.myportfolio.com/animatronic-eyes

---

## TL;DR
Most eye tutorials stop at “the servos move.” This build focuses on the missing layer:
- **Motion quality:** smooth following (ease-like behavior) instead of snapping
- **Behavior polish:** startup choreography, idle animations, randomized blinking
- **Maintainability:** calibration tooling + config-driven parameters
- **Safety:** runtime constraint enforcement to prevent limit violations

---

## Features
### Motion & Expressions
- 6-axis control: **H/V + 4 independent eyelids**
- Smooth interpolation (configurable smoothing factor)
- Eyelid position animation (supports partial closure / expression presets later)

### Behaviors
- **Startup sequence** (~5.8s): close → hold → open → look-around → center
- **Active mode:** manual control via Wii Nunchuck
- **Idle mode:** activates after inactivity; randomized gaze patterns
- **Auto-blink:** randomized interval for natural timing variation

### Reliability & Maintenance
- Per-channel servo calibration (min/max pulse bounds)
- Safe limits enforced at runtime
- Servo inversion flags (software handles inverted mounting; no rewiring)
- Deadzone filtering to eliminate joystick jitter near center

---

## Hardware
### Bill of Materials (BOM)
- Arduino UNO R3
- Adafruit 16-Channel PWM Servo Shield (PCA9685)
- 6× SG90 micro servos
- Wii Nunchuck (I2C)
- Dedicated 5V 4A power supply (recommended)
- 3D printed dual eye mechanism

### Wiring Overview

- Servo Shield: I2C address (e.g., `0x44` with A2 bridged)
- Nunchuck: I2C SDA/SCL to Arduino (level shifting if needed)
- Power: external 5V to servo rail, **common ground with Arduino**

**Important:** Do not power 6 servos from USB. Use external 5V supply.

---

## Mechanical Notes
- Dual-eye mechanism with shared H/V actuation
- Independent upper/lower eyelids per eye
- Some servos mount inverted depending on geometry (handled in software)

---

## Calibration-First Workflow (Recommended)
### Why calibration matters
SG90 servos vary and mechanical limits vary. Driving into hard stops causes buzzing, heat, and failure.
This project uses a per-channel calibration step to define safe pulse bounds.

### ServoPulseCalibrator.ino
**What it does:**
- Interactively sets min/max pulse widths per servo channel via Serial
- Enforces absolute safety limits while tuning
- Supports coarse and fine step sizes for faster calibration

**Workflow**
1. Upload `ServoPulseCalibrator.ino`
2. Test each channel individually (one servo at a time recommended)
3. Record calibrated min/max pulse values
4. Copy values into `EyeConfig.h`
5. Upload main controller sketch

---

## Software Architecture
### State Machine
- `STATE_STARTUP` — choreographed boot routine
- `STATE_ACTIVE` — manual joystick control
- `STATE_IDLE` — autonomous behaviors

**Key design detail:** activity detection runs **before** state handling to ensure instant response when exiting idle mode.

### Configuration-Driven Design (EyeConfig.h)
Single source of truth for:
- servo channels + calibration limits
- inversion flags per servo
- smoothing factor
- deadzone
- idle timing + blink timing

---

## Motion Model
### Smooth Following (Ease-like behavior)
Instead of snapping to targets, positions follow smoothly:
- `current = smooth(current, target, smoothingFactor)`
- smoothing factor typical: 8–10

### Eyelids (Position-Based)
Eyelids transition through intermediate positions:
- 0.0 open
- 0.5 half
- 1.0 closed

Enables smooth blinking and future expression presets.

---

## Behaviors (Details)
### Startup Sequence (~5.8 seconds)
1) Close (1.0s)  
2) Hold (1.0s)  
3) Open (0.8s)  
4) Look-around L → R → U → D (2.0s)  
5) Center (0.5s)

### Idle Mode
- Activates after `IDLE_TIMEOUT` (default ~15s)
- Random gaze target every 2–4s
- Auto-blink every 2–6s (randomized)

---

## Getting Started
### Requirements
- Arduino IDE
- Adafruit PWM Servo Driver library
- Wire / I2C helpers as needed (Nunchuck library, depending on implementation)

### Build & Upload
1. Clone repo
2. Install required libraries
3. Upload `ServoPulseCalibrator.ino` and calibrate
4. Update `EyeConfig.h` with your calibrated values
5. Upload main sketch
6. Power with external 5V supply and test startup/idle

---

## Troubleshooting
### Servo buzzes / gets hot
- Reduce pulse range (calibrate again)
- Check for mechanical binding
- Verify external power + common ground

### Eyes jitter at center
- Increase deadzone
- Confirm Nunchuck neutral values
- Add smoothing / reduce sensitivity

### Idle animations don’t run
- Confirm activity detection order (should run before state logic)
- Enable serial debug output for state + targets

### One servo moves opposite direction
- Set inversion flag for that channel in config (no rewiring required)

---

## Roadmap / Next Steps
- Expression presets (surprised / sleepy / suspicious / angry)
- Vision tracking (RealSense) for attention behaviors
- ROS 2 architecture for multi-figure coordination
- Sound-reactive behaviors (directional attention)
- Non-linear easing for eyelids (cubic interpolation)

---

## Credits
- Mechanism inspiration: [Will Cogley (Simplified Eye Mechanism)](https://www.instructables.com/Simplified-3D-Printed-Animatronic-Dual-Eye-Mechani/)
- Libraries: Adafruit PWM Servo Driver, (Nunchuck library link)

---

## License
- MIT 
