/**
 * Nunchuck Characterization Tool - Min/Max Tracker
 * 
 * This tool tracks and displays the minimum and maximum values for all nunchuck inputs
 * Great for understanding your specific nunchuck's range!
 * 
 * LIBRARY REQUIRED:
 * 1. Arduino IDE → Sketch → Include Library → Manage Libraries
 * 2. Search: "WiiChuck"
 * 3. Install: "WiiChuck" by madhephaestus
 * 
 * Hardware:
 * - Arduino UNO
 * - Wii Nunchuck with I2C adapter
 * - Connect: SDA → A4, SCL → A5, VCC → 3.3V or 5V, GND → GND
 * 
 * INSTRUCTIONS:
 * 1. Upload this sketch
 * 2. Open Serial Monitor (115200 baud)
 * 3. Move joystick to ALL extremes (up, down, left, right, diagonals)
 * 4. Tilt nunchuck in all directions (for accelerometer)
 * 5. Press both buttons
 * 6. Watch min/max values update
 */

#include <Wire.h>
#include <WiiChuck.h>

// Create nunchuck object
Accessory nunchuck;

// Current values
int joyX = 0;
int joyY = 0;
int joyAccX = 0;
int joyAccY = 0;
int joyAccZ = 0;
bool buttonZ = false;
bool buttonC = false;

// Min/Max tracking variables - initialized to opposite extremes
int joyX_min = 32767;   // Start at max int
int joyX_max = -32768;  // Start at min int
int joyY_min = 32767;
int joyY_max = -32768;

int joyAccX_min = 32767;
int joyAccX_max = -32768;
int joyAccY_min = 32767;
int joyAccY_max = -32768;
int joyAccZ_min = 32767;
int joyAccZ_max = -32768;

// Button tracking (count how many times pressed)
int buttonZ_count = 0;
int buttonC_count = 0;
bool buttonZ_lastState = false;
bool buttonC_lastState = false;

void setup() {
  Serial.begin(115200);
  
  Serial.println(F("\n\n========================================"));
  Serial.println(F("Wii Nunchuck Characterization Tool"));
  Serial.println(F("Min/Max Value Tracker"));
  Serial.println(F("========================================\n"));
  
  // Initialize nunchuck
  nunchuck.begin();
  
  // Wait for nunchuck to be ready
  Serial.println(F("Waiting for nunchuck..."));
  while (nunchuck.type != NUNCHUCK) {
    nunchuck.readData();
    delay(100);
  }
  
  Serial.println(F("Nunchuck connected!\n"));
  Serial.println(F("INSTRUCTIONS:"));
  Serial.println(F("1. Move joystick to ALL extremes"));
  Serial.println(F("2. Tilt nunchuck in all directions"));
  Serial.println(F("3. Press both buttons"));
  Serial.println(F("4. Watch min/max values update\n"));
  
  delay(2000);
}

void loop() {
  // Update nunchuck data
  nunchuck.readData();
  
  // Get current joystick values
  joyX = nunchuck.getJoyX();
  joyY = nunchuck.getJoyY();
  
  // Get current accelerometer values
  joyAccX = nunchuck.getAccelX();
  joyAccY = nunchuck.getAccelY();
  joyAccZ = nunchuck.getAccelZ();
  
  // Get button states
  buttonZ = nunchuck.getButtonZ();
  buttonC = nunchuck.getButtonC();
  
  // Update min/max for joystick X
  if (joyX < joyX_min) joyX_min = joyX;
  if (joyX > joyX_max) joyX_max = joyX;
  
  // Update min/max for joystick Y
  if (joyY < joyY_min) joyY_min = joyY;
  if (joyY > joyY_max) joyY_max = joyY;
  
  // Update min/max for accelerometer X
  if (joyAccX < joyAccX_min) joyAccX_min = joyAccX;
  if (joyAccX > joyAccX_max) joyAccX_max = joyAccX;
  
  // Update min/max for accelerometer Y
  if (joyAccY < joyAccY_min) joyAccY_min = joyAccY;
  if (joyAccY > joyAccY_max) joyAccY_max = joyAccY;
  
  // Update min/max for accelerometer Z
  if (joyAccZ < joyAccZ_min) joyAccZ_min = joyAccZ;
  if (joyAccZ > joyAccZ_max) joyAccZ_max = joyAccZ;
  
  // Count button presses (detect rising edge)
  if (buttonZ && !buttonZ_lastState) {
    buttonZ_count++;
  }
  if (buttonC && !buttonC_lastState) {
    buttonC_count++;
  }
  buttonZ_lastState = buttonZ;
  buttonC_lastState = buttonC;
  
  // Print current values and min/max
  printCharacterization();
  
  delay(100);
}

void printCharacterization() {
  // Clear screen (optional - comment out if it's too jumpy)
  // Serial.write(27);       // ESC
  // Serial.print("[2J");    // Clear screen
  // Serial.write(27);       // ESC
  // Serial.print("[H");     // Home cursor
  
  Serial.println(F("========================================"));
  Serial.println(F("CURRENT VALUES"));
  Serial.println(F("========================================"));
  Serial.println(F("Joy-X  Joy-Y    |  Z-Btn   C-Btn   |  AccX  AccY  AccZ"));
  Serial.println(F("----------------------------------------------------------"));
  
  // Current values
  Serial.print(joyX);
  Serial.print("\t");
  Serial.print(joyY);
  Serial.print("\t| ");
  
  if (buttonZ) {
    Serial.print("PRESSED ");
  } else {
    Serial.print("------- ");
  }
  
  if (buttonC) {
    Serial.print(" PRESSED |  ");
  } else {
    Serial.print(" ------- |  ");
  }
  
  Serial.print(joyAccX);
  Serial.print("   ");
  Serial.print(joyAccY);
  Serial.print("   ");
  Serial.print(joyAccZ);
  
  Serial.println("\n");
  
  // Min/Max Values Section
  Serial.println(F("========================================"));
  Serial.println(F("MIN/MAX VALUES RECORDED"));
  Serial.println(F("========================================"));
  
  // Joystick X
  Serial.print(F("Joy-X:  Min = "));
  Serial.print(joyX_min);
  Serial.print(F("\tMax = "));
  Serial.print(joyX_max);
  Serial.print(F("\tRange = "));
  Serial.println(joyX_max - joyX_min);
  
  // Joystick Y
  Serial.print(F("Joy-Y:  Min = "));
  Serial.print(joyY_min);
  Serial.print(F("\tMax = "));
  Serial.print(joyY_max);
  Serial.print(F("\tRange = "));
  Serial.println(joyY_max - joyY_min);
  
  Serial.println();
  
  // Accelerometer X
  Serial.print(F("Acc-X:  Min = "));
  Serial.print(joyAccX_min);
  Serial.print(F("\tMax = "));
  Serial.print(joyAccX_max);
  Serial.print(F("\tRange = "));
  Serial.println(joyAccX_max - joyAccX_min);
  
  // Accelerometer Y
  Serial.print(F("Acc-Y:  Min = "));
  Serial.print(joyAccY_min);
  Serial.print(F("\tMax = "));
  Serial.print(joyAccY_max);
  Serial.print(F("\tRange = "));
  Serial.println(joyAccY_max - joyAccY_min);
  
  // Accelerometer Z
  Serial.print(F("Acc-Z:  Min = "));
  Serial.print(joyAccZ_min);
  Serial.print(F("\tMax = "));
  Serial.print(joyAccZ_max);
  Serial.print(F("\tRange = "));
  Serial.println(joyAccZ_max - joyAccZ_min);
  
  Serial.println();
  
  // Button press counts
  Serial.print(F("Button Z pressed: "));
  Serial.print(buttonZ_count);
  Serial.println(F(" times"));
  
  Serial.print(F("Button C pressed: "));
  Serial.print(buttonC_count);
  Serial.println(F(" times"));
  
  Serial.println(F("========================================\n\n"));
}
