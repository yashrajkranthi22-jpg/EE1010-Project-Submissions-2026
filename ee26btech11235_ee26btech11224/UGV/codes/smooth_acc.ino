/*
=========================================================
UGV PART 1 - TASK 6
SMOOTH ACCELERATION AND DECELERATION

Microcontroller : ESP32 DevKit V1
Motor Driver    : L293D
Remote Control  : Dabble Bluetooth GamePad

PWM Resolution  : 8-bit (0-255)
PWM Frequency   : 2000 Hz

RAMP DURATION   : 1.5 seconds (0 to 255)
SLEW RATE       : 170 PWM units/second

SPEED BUTTONS:
Square   : PWM +10
Triangle : PWM +1
Circle   : PWM -10
Cross    : PWM -1

MOVEMENT BUTTONS:
UP       : Forward
DOWN     : Backward
LEFT     : Left pivot
RIGHT    : Right pivot

SAFETY:
START    : Emergency stop (latched)
SELECT   : Reset emergency stop

=========================================================
*/


// =====================================================
// 1. DABBLE CONFIGURATION
// =====================================================

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

#include <Arduino.h>
#include <DabbleESP32.h>
#include <esp_arduino_version.h>
#include <math.h>

// Internal BLE connection flag in DabbleESP32.
// Its onConnect/onDisconnect callbacks update this variable.
extern bool isDeviceConnected;


// =====================================================
// 2. MOTOR GPIO CONFIGURATION
// =====================================================

// LEFT MOTOR

const int LEFT_PWM_PIN = 18;
const int LEFT_IN1 = 32;
const int LEFT_IN2 = 33;

// RIGHT MOTOR

const int RIGHT_PWM_PIN = 19;
const int RIGHT_IN1 = 25;
const int RIGHT_IN2 = 26;


// =====================================================
// 3. PWM CONFIGURATION
// =====================================================

const int PWM_FREQUENCY = 2000;
const int PWM_RESOLUTION = 8;

const int PWM_MAX = 255;

// ESP32 Core 2.x PWM channels
const int LEFT_PWM_CHANNEL = 0;
const int RIGHT_PWM_CHANNEL = 1;

bool pwmReady = false;


// =====================================================
// 4. TASK 6: RAMP CONFIGURATION
// =====================================================

// Time taken to move from PWM 0 to PWM 255

const float RAMP_DURATION = 1.5f;

// Maximum PWM change per second

const float PWM_SLEW_RATE = PWM_MAX / RAMP_DURATION;

// 170 PWM units per second

// Limit a single update to 50 ms worth of ramping.
// This prevents a large PWM jump if the loop is delayed.

const float MAX_UPDATE_INTERVAL = 0.050f;

unsigned long previousRampTime = 0;


// =====================================================
// 5. REQUIRED MOTOR VARIABLES
// =====================================================

// LEFT MOTOR

float currentPWMLeft = 0.0f;

int targetPWMLeft = 0;


// RIGHT MOTOR

float currentPWMRight = 0.0f;

int targetPWMRight = 0;


// =====================================================
// 6. MANUAL PWM SPEED CONTROL
// =====================================================

// Speed adjustment starts at zero.

int speedCommandPWM = 0;

const int PWM_INCREMENT_LARGE = 10;
const int PWM_INCREMENT_SMALL = 1;


// =====================================================
// 7. BUTTON STATE VARIABLES
// =====================================================

// Used to detect individual button presses.

bool previousSquare = false;
bool previousTriangle = false;

bool previousCircle = false;
bool previousCross = false;

bool previousSelect = false;


// =====================================================
// 8. SAFETY VARIABLES
// =====================================================

// Emergency stop remains active until SELECT is pressed.

bool emergencyStopLatched = false;

// Require directional buttons to be released after
// Bluetooth reconnects or emergency-stop reset.

bool motionArmed = false;

// BLE connection tracking

bool previousBLEConnection = false;


// =====================================================
// 9. PWM INITIALIZATION
// =====================================================

bool initializePWM() {

#if ESP_ARDUINO_VERSION_MAJOR >= 3

  bool leftReady = ledcAttach(
    LEFT_PWM_PIN,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );

  bool rightReady = ledcAttach(
    RIGHT_PWM_PIN,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );

  return leftReady && rightReady;

#else

  ledcSetup(
    LEFT_PWM_CHANNEL,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );

  ledcSetup(
    RIGHT_PWM_CHANNEL,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );

  ledcAttachPin(
    LEFT_PWM_PIN,
    LEFT_PWM_CHANNEL
  );

  ledcAttachPin(
    RIGHT_PWM_PIN,
    RIGHT_PWM_CHANNEL
  );

  return true;

#endif

}


// =====================================================
// 10. PWM OUTPUT FUNCTION
// =====================================================

void writeMotorPWM(
  int pwmPin,
  int pwmChannel,
  int duty
) {

  duty = constrain(duty, 0, PWM_MAX);

#if ESP_ARDUINO_VERSION_MAJOR >= 3

  ledcWrite(pwmPin, duty);

#else

  ledcWrite(pwmChannel, duty);

#endif

}


// =====================================================
// 11. LEFT MOTOR OUTPUT
// =====================================================

void applyLeftMotorOutput() {

  int duty = constrain(
    (int)lroundf(fabsf(currentPWMLeft)),
    0,
    PWM_MAX
  );


  if (duty == 0) {

    writeMotorPWM(
      LEFT_PWM_PIN,
      LEFT_PWM_CHANNEL,
      0
    );

    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);

  }

  else if (currentPWMLeft > 0) {

    // Forward

    digitalWrite(LEFT_IN1, HIGH);
    digitalWrite(LEFT_IN2, LOW);

    writeMotorPWM(
      LEFT_PWM_PIN,
      LEFT_PWM_CHANNEL,
      duty
    );

  }

  else {

    // Reverse

    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, HIGH);

    writeMotorPWM(
      LEFT_PWM_PIN,
      LEFT_PWM_CHANNEL,
      duty
    );

  }

}


// =====================================================
// 12. RIGHT MOTOR OUTPUT
// =====================================================

void applyRightMotorOutput() {

  int duty = constrain(
    (int)lroundf(fabsf(currentPWMRight)),
    0,
    PWM_MAX
  );


  if (duty == 0) {

    writeMotorPWM(
      RIGHT_PWM_PIN,
      RIGHT_PWM_CHANNEL,
      0
    );

    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, LOW);

  }

  else if (currentPWMRight > 0) {

    // Forward

    digitalWrite(RIGHT_IN1, HIGH);
    digitalWrite(RIGHT_IN2, LOW);

    writeMotorPWM(
      RIGHT_PWM_PIN,
      RIGHT_PWM_CHANNEL,
      duty
    );

  }

  else {

    // Reverse

    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, HIGH);

    writeMotorPWM(
      RIGHT_PWM_PIN,
      RIGHT_PWM_CHANNEL,
      duty
    );

  }

}


// =====================================================
// 13. IMMEDIATE MOTOR STOP
// =====================================================

// Emergency and Bluetooth-loss stops bypass ramping.

void immediateMotorStop() {

  targetPWMLeft = 0;
  targetPWMRight = 0;

  currentPWMLeft = 0.0f;
  currentPWMRight = 0.0f;

  if (pwmReady) {

    applyLeftMotorOutput();
    applyRightMotorOutput();

  }

}


// =====================================================
// 14. PWM SPEED ADJUSTMENT
// =====================================================

void updateSpeedControl() {

  bool square = GamePad.isSquarePressed();

  bool triangle = GamePad.isTrianglePressed();

  bool circle = GamePad.isCirclePressed();

  bool cross = GamePad.isCrossPressed();


  // SQUARE: Increase by 10

  if (square && !previousSquare) {

    speedCommandPWM += PWM_INCREMENT_LARGE;

  }

  // TRIANGLE: Increase by 1

  else if (triangle && !previousTriangle) {

    speedCommandPWM += PWM_INCREMENT_SMALL;

  }

  // CIRCLE: Decrease by 10

  else if (circle && !previousCircle) {

    speedCommandPWM -= PWM_INCREMENT_LARGE;

  }

  // CROSS: Decrease by 1

  else if (cross && !previousCross) {

    speedCommandPWM -= PWM_INCREMENT_SMALL;

  }


  speedCommandPWM = constrain(
    speedCommandPWM,
    0,
    PWM_MAX
  );


  previousSquare = square;
  previousTriangle = triangle;

  previousCircle = circle;
  previousCross = cross;

}


// =====================================================
// 15. EMERGENCY STOP CONTROL
// =====================================================

void updateEmergencyStop() {

  bool startPressed = GamePad.isStartPressed();

  bool selectPressed = GamePad.isSelectPressed();


  // START: Activate emergency stop

  if (startPressed) {

    if (!emergencyStopLatched) {

      emergencyStopLatched = true;

      immediateMotorStop();

      motionArmed = false;

      Serial.println();
      Serial.println("!!! EMERGENCY STOP ACTIVATED !!!");

    }

  }


  // SELECT: Reset emergency stop
  // START must first be released.

  else if (
    selectPressed &&
    !previousSelect &&
    emergencyStopLatched
  ) {

    emergencyStopLatched = false;

    motionArmed = false;

    Serial.println("Emergency stop reset.");

    Serial.println(
      "Release all direction buttons before moving."
    );

  }


  previousSelect = selectPressed;

}


// =====================================================
// 16. UPDATE MOVEMENT TARGETS
// =====================================================

void updateMovementTargets() {

  // Stop if emergency stop is active.

  if (emergencyStopLatched) {

    targetPWMLeft = 0;
    targetPWMRight = 0;

    return;

  }


  bool up = GamePad.isUpPressed();

  bool down = GamePad.isDownPressed();

  bool left = GamePad.isLeftPressed();

  bool right = GamePad.isRightPressed();


  // Require all directional buttons to be released
  // after startup, reconnect or emergency-stop reset.

  if (!motionArmed) {

    if (!up && !down && !left && !right) {

      motionArmed = true;

      Serial.println("Movement controls armed.");

    }

    targetPWMLeft = 0;
    targetPWMRight = 0;

    return;

  }


  // FORWARD

  if (up && !down && !left && !right) {

    targetPWMLeft = speedCommandPWM;

    targetPWMRight = speedCommandPWM;

  }


  // BACKWARD

  else if (down && !up && !left && !right) {

    targetPWMLeft = -speedCommandPWM;

    targetPWMRight = -speedCommandPWM;

  }


  // LEFT PIVOT

  else if (left && !right && !up && !down) {

    targetPWMLeft = -speedCommandPWM;

    targetPWMRight = speedCommandPWM;

  }


  // RIGHT PIVOT

  else if (right && !left && !up && !down) {

    targetPWMLeft = speedCommandPWM;

    targetPWMRight = -speedCommandPWM;

  }


  // STOP

  else {

    targetPWMLeft = 0;

    targetPWMRight = 0;

  }

}


// =====================================================
// 17. SLEW RATE LIMITER
// =====================================================

// Moves current PWM towards target PWM.
//
// If direction reverses, the intermediate target
// becomes zero. The motor must reach zero before
// the opposite direction is applied.

float slewPWM(
  float currentPWM,
  int targetPWM,
  float maximumChange
) {

  float effectiveTarget = (float)targetPWM;


  // Direction reversal protection

  if (
    (currentPWM > 0.0f && targetPWM < 0) ||
    (currentPWM < 0.0f && targetPWM > 0)
  ) {

    effectiveTarget = 0.0f;

  }


  float difference = effectiveTarget - currentPWM;


  // Target reached

  if (fabsf(difference) <= maximumChange) {

    return effectiveTarget;

  }


  // Increase PWM gradually

  if (difference > 0.0f) {

    currentPWM += maximumChange;

  }


  // Decrease PWM gradually

  else {

    currentPWM -= maximumChange;

  }


  return currentPWM;

}


// =====================================================
// 18. TASK 6: UPDATE PWM RAMPING
// =====================================================

void updateMotorRamp() {

  unsigned long currentTime = micros();


  if (previousRampTime == 0) {

    previousRampTime = currentTime;

    return;

  }


  // Calculate actual elapsed time

  unsigned long elapsedMicros =
    currentTime - previousRampTime;

  previousRampTime = currentTime;


  float elapsedTime =
    elapsedMicros / 1000000.0f;


  // Prevent a large PWM jump after a delayed loop.

  if (elapsedTime > MAX_UPDATE_INTERVAL) {

    elapsedTime = MAX_UPDATE_INTERVAL;

  }


  // Calculate permitted PWM change.

  float maximumChange =
    PWM_SLEW_RATE * elapsedTime;


  // Update LEFT motor independently.

  currentPWMLeft = slewPWM(
    currentPWMLeft,
    targetPWMLeft,
    maximumChange
  );


  // Update RIGHT motor independently.

  currentPWMRight = slewPWM(
    currentPWMRight,
    targetPWMRight,
    maximumChange
  );


  // Apply updated PWM.

  applyLeftMotorOutput();

  applyRightMotorOutput();

}


// =====================================================
// 19. BLE CONNECTION MONITOR
// =====================================================

void updateBLEConnection() {

  bool connected = isDeviceConnected;


  // Bluetooth disconnected

  if (!connected) {

    if (previousBLEConnection) {

      Serial.println();
      Serial.println("!!! BLUETOOTH DISCONNECTED !!!");

      Serial.println("Stopping both motors immediately.");

    }

    if (
      previousBLEConnection ||
      currentPWMLeft != 0.0f ||
      currentPWMRight != 0.0f ||
      targetPWMLeft != 0 ||
      targetPWMRight != 0
    ) {

      immediateMotorStop();

    }

    previousBLEConnection = false;

    motionArmed = false;

    return;

  }


  // Bluetooth newly connected

  if (!previousBLEConnection) {

    previousBLEConnection = true;

    motionArmed = false;

    Serial.println();
    Serial.println("Bluetooth connected successfully.");

    Serial.println(
      "Release directional buttons before moving."
    );

  }

}


// =====================================================
// 20. SERIAL MONITOR STATUS
// =====================================================

void printStatus() {

  static unsigned long previousPrint = 0;

  if (millis() - previousPrint < 250) {

    return;

  }

  previousPrint = millis();


  Serial.print("Speed Command: ");

  Serial.print(speedCommandPWM);


  Serial.print(" | Target L: ");

  Serial.print(targetPWMLeft);


  Serial.print(" | Current L: ");

  Serial.print(currentPWMLeft, 1);


  Serial.print(" | Target R: ");

  Serial.print(targetPWMRight);


  Serial.print(" | Current R: ");

  Serial.print(currentPWMRight, 1);


  Serial.print(" | E-STOP: ");

  Serial.println(
    emergencyStopLatched ? "ACTIVE" : "OFF"
  );

}


// =====================================================
// 21. SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(500);


  Serial.println();

  Serial.println("========================================");

  Serial.println("UGV PART 1 - TASK 6");

  Serial.println("SMOOTH ACCELERATION AND DECELERATION");

  Serial.println("========================================");


  // Configure direction pins

  pinMode(LEFT_IN1, OUTPUT);

  pinMode(LEFT_IN2, OUTPUT);

  pinMode(RIGHT_IN1, OUTPUT);

  pinMode(RIGHT_IN2, OUTPUT);


  // Initially disable both motors

  digitalWrite(LEFT_IN1, LOW);

  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(RIGHT_IN1, LOW);

  digitalWrite(RIGHT_IN2, LOW);


  // Initialize PWM

  Serial.println("Initializing motor PWM...");

  pwmReady = initializePWM();


  if (pwmReady) {

    writeMotorPWM(LEFT_PWM_PIN, LEFT_PWM_CHANNEL, 0);

    writeMotorPWM(RIGHT_PWM_PIN, RIGHT_PWM_CHANNEL, 0);

    Serial.println("PWM initialized successfully.");

  }

  else {

    Serial.println("ERROR: PWM initialization failed!");

    Serial.println("Motor operation disabled.");

  }


  // Initialize Dabble BLE

  Serial.println("Initializing Dabble BLE...");

  Dabble.begin("UGV_ESP32");


  Serial.println();

  Serial.println("PWM CALIBRATION AND CONTROL");

  Serial.println("Initial speed command: 0");

  Serial.println("Maximum PWM: 255");

  Serial.println();

  Serial.print("Ramp duration: ");

  Serial.print(RAMP_DURATION);

  Serial.println(" seconds");


  Serial.print("PWM slew rate: ");

  Serial.print(PWM_SLEW_RATE);

  Serial.println(" units/second");


  Serial.println();

  Serial.println("SPEED CONTROLS:");

  Serial.println("SQUARE   : PWM +10");

  Serial.println("TRIANGLE : PWM +1");

  Serial.println("CIRCLE   : PWM -10");

  Serial.println("CROSS    : PWM -1");


  Serial.println();

  Serial.println("MOVEMENT CONTROLS:");

  Serial.println("UP       : FORWARD");

  Serial.println("DOWN     : BACKWARD");

  Serial.println("LEFT     : LEFT PIVOT");

  Serial.println("RIGHT    : RIGHT PIVOT");


  Serial.println();

  Serial.println("SAFETY CONTROLS:");

  Serial.println("START    : EMERGENCY STOP");

  Serial.println("SELECT   : RESET EMERGENCY STOP");


  Serial.println();

  Serial.println("========================================");

}


// =====================================================
// 22. MAIN LOOP
// =====================================================

void loop() {

  // 1. Process incoming Bluetooth data.

  Dabble.processInput();


  // 2. Check BLE connection.

  updateBLEConnection();


  // Do not operate motors if PWM or BLE is unavailable.

  if (!pwmReady || !isDeviceConnected) {

    return;

  }


  // 3. Process emergency stop.

  updateEmergencyStop();


  // 4. Emergency stop bypasses normal ramping.

  if (emergencyStopLatched) {

    immediateMotorStop();

    printStatus();

    return;

  }


  // 5. Adjust requested speed.

  updateSpeedControl();


  // 6. Calculate independent motor targets.

  updateMovementTargets();


  // 7. Apply time-based PWM slew rate.

  updateMotorRamp();


  // 8. Display motor information.

  printStatus();

}