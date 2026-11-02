/*
=========================================================
UGV PART 1 - TASK 5
DIFFERENT SPEED SETTINGS

Microcontroller : ESP32 DevKit V1
Motor Driver    : L293D
Remote Control  : Dabble Bluetooth GamePad

SLOW   = 100
MEDIUM = 160
FAST   = 220

PWM Frequency  = 2000 Hz
PWM Resolution = 8 bits
=========================================================
*/

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

#include <DabbleESP32.h>

// =====================================================
// 1. MOTOR GPIO CONFIGURATION
// =====================================================

// Left motor
#define LEFT_PWM    18
#define LEFT_IN1    32
#define LEFT_IN2    33

// Right motor
#define RIGHT_PWM   19
#define RIGHT_IN1   25
#define RIGHT_IN2   26


// =====================================================
// 2. PWM CONFIGURATION
// =====================================================

#define PWM_FREQUENCY  2000
#define PWM_RESOLUTION 8

bool pwmReady = false;


// =====================================================
// 3. SPEED PROFILES
// =====================================================

#define SLOW_PWM    100
#define MEDIUM_PWM  160
#define FAST_PWM    220

int requestedPWM = MEDIUM_PWM;

const char* selectedSpeed = "MEDIUM";


// =====================================================
// 4. MOTOR VARIABLES
// =====================================================

int targetLeftPWM = 0;
int targetRightPWM = 0;

int currentLeftPWM = 0;
int currentRightPWM = 0;


// =====================================================
// 5. RAMPING CONFIGURATION
// =====================================================

#define RAMP_STEP       5
#define RAMP_INTERVAL   10

unsigned long previousRampTime = 0;


// =====================================================
// 6. SPEED SELECTION
// =====================================================

void updateSpeedSelection() {

  if (GamePad.isSquarePressed()) {

    requestedPWM = SLOW_PWM;
    selectedSpeed = "SLOW";

  }

  else if (GamePad.isCirclePressed()) {

    requestedPWM = MEDIUM_PWM;
    selectedSpeed = "MEDIUM";

  }

  else if (GamePad.isTrianglePressed()) {

    requestedPWM = FAST_PWM;
    selectedSpeed = "FAST";

  }

}


// =====================================================
// 7. MOVEMENT CONTROL
// =====================================================

void updateDriveCommands() {

  targetLeftPWM = 0;
  targetRightPWM = 0;

  if (GamePad.isUpPressed()) {

    targetLeftPWM = requestedPWM;
    targetRightPWM = requestedPWM;

  }

  else if (GamePad.isDownPressed()) {

    targetLeftPWM = -requestedPWM;
    targetRightPWM = -requestedPWM;

  }

  else if (GamePad.isLeftPressed()) {

    targetLeftPWM = -requestedPWM;
    targetRightPWM = requestedPWM;

  }

  else if (GamePad.isRightPressed()) {

    targetLeftPWM = requestedPWM;
    targetRightPWM = -requestedPWM;

  }

}


// =====================================================
// 8. LEFT MOTOR OUTPUT
// =====================================================

void setLeftMotor(int pwm) {

  if (pwm > 0) {

    digitalWrite(LEFT_IN1, HIGH);
    digitalWrite(LEFT_IN2, LOW);

  }

  else if (pwm < 0) {

    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, HIGH);

  }

  else {

    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);

  }

  ledcWrite(LEFT_PWM, abs(pwm));

}


// =====================================================
// 9. RIGHT MOTOR OUTPUT
// =====================================================

void setRightMotor(int pwm) {

  if (pwm > 0) {

    digitalWrite(RIGHT_IN1, HIGH);
    digitalWrite(RIGHT_IN2, LOW);

  }

  else if (pwm < 0) {

    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, HIGH);

  }

  else {

    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, LOW);

  }

  ledcWrite(RIGHT_PWM, abs(pwm));

}


// =====================================================
// 10. SMOOTH PWM RAMPING
// =====================================================

int rampMotor(int currentPWM, int targetPWM) {

  // Stop before reversing direction

  if (currentPWM * targetPWM < 0) {

    if (currentPWM > 0) {

      currentPWM -= RAMP_STEP;

      if (currentPWM < 0) {
        currentPWM = 0;
      }

    }

    else {

      currentPWM += RAMP_STEP;

      if (currentPWM > 0) {
        currentPWM = 0;
      }

    }

    return currentPWM;

  }

  // Gradually increase PWM

  if (currentPWM < targetPWM) {

    currentPWM += RAMP_STEP;

    if (currentPWM > targetPWM) {
      currentPWM = targetPWM;
    }

  }

  // Gradually decrease PWM

  else if (currentPWM > targetPWM) {

    currentPWM -= RAMP_STEP;

    if (currentPWM < targetPWM) {
      currentPWM = targetPWM;
    }

  }

  return currentPWM;

}


// =====================================================
// 11. UPDATE MOTOR OUTPUTS
// =====================================================

void updateMotorRamp() {

  unsigned long now = millis();

  if (now - previousRampTime >= RAMP_INTERVAL) {

    previousRampTime = now;

    currentLeftPWM = rampMotor(
      currentLeftPWM,
      targetLeftPWM
    );

    currentRightPWM = rampMotor(
      currentRightPWM,
      targetRightPWM
    );

    setLeftMotor(currentLeftPWM);
    setRightMotor(currentRightPWM);

  }

}


// =====================================================
// 12. SERIAL MONITOR
// =====================================================

void printStatus() {

  static int previousPWM = -1;

  if (requestedPWM != previousPWM) {

    Serial.println();
    Serial.println("========== SPEED PROFILE ==========");

    Serial.print("Selected Mode: ");
    Serial.println(selectedSpeed);

    Serial.print("Requested PWM: ");
    Serial.println(requestedPWM);

    Serial.println("===================================");

    previousPWM = requestedPWM;

  }

}


// =====================================================
// 13. SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("Starting UGV Task 5...");

  // Configure motor direction pins

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  // Keep motors stopped

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);


  // ---------------------------------------------
  // IMPORTANT: START DABBLE BEFORE PWM SETUP
  // ---------------------------------------------

  Serial.println("Initializing Dabble BLE...");

  Dabble.begin("UGV_ESP32");

  Serial.println("Dabble BLE initialization requested.");
  Serial.println("Device name: UGV_ESP32");


  // ---------------------------------------------
  // INITIALIZE MOTOR PWM
  // ---------------------------------------------

  Serial.println("Initializing motor PWM...");

  bool leftPWMReady = ledcAttach(
    LEFT_PWM,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );

  bool rightPWMReady = ledcAttach(
    RIGHT_PWM,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );

  pwmReady = leftPWMReady && rightPWMReady;


  if (pwmReady) {

    ledcWrite(LEFT_PWM, 0);
    ledcWrite(RIGHT_PWM, 0);

    Serial.println("Motor PWM initialized successfully.");

  }

  else {

    Serial.println("WARNING: Motor PWM initialization failed.");
    Serial.println("Bluetooth will remain active.");
    Serial.println("Motors are disabled until PWM is fixed.");

  }


  Serial.println();
  Serial.println("=================================");
  Serial.println("UGV TASK 5 READY");
  Serial.println("Default Speed: MEDIUM");
  Serial.println("Requested PWM: 160");
  Serial.println("=================================");

}


// =====================================================
// 14. MAIN LOOP
// =====================================================

void loop() {

  // 1. Process Bluetooth communication
  Dabble.processInput();

  // 2. Update speed profile
  updateSpeedSelection();

  // 3. Update movement commands
  updateDriveCommands();

  // 4. Update motor outputs only if PWM is ready
  if (pwmReady) {

    updateMotorRamp();

  }

  // 5. Display selected speed
  printStatus();

}