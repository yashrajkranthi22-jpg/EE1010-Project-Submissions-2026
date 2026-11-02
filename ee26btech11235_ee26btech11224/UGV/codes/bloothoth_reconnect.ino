/*
=========================================================
UGV PART 1 - COMBINED FIRMWARE

TASK 3 : Independent PWM Motor Control
TASK 4 : Smooth Motor Control Improvements
TASK 5 : Adjustable Speed Control
TASK 6 : Smooth Acceleration and Deceleration
TASK 8 : Bluetooth-Loss Stopping

Microcontroller : ESP32 DevKit V1
Motor Driver    : L293D
Communication   : Dabble BLE
PWM Resolution  : 8-bit (0-255)
PWM Frequency   : 2000 Hz

RAMP DURATION   : 1.5 seconds (0 to 255)
SLEW RATE       : 170 PWM units/second

SPEED CONTROLS:
Square   : PWM +10
Triangle : PWM +1
Circle   : PWM -10
Cross    : PWM -1

MOVEMENT CONTROLS:
UP       : Forward
DOWN     : Backward
LEFT     : Left Pivot
RIGHT    : Right Pivot

SAFETY:
START    : Emergency Stop
SELECT   : Reset Emergency Stop

BLUETOOTH:
Disconnect : Immediate motor stop
Reconnect  : Require neutral input before movement

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
#include <BLEServer.h>
#include <math.h>


// DabbleESP32 internal BLE state.
//
// These globals are defined by esp32BLEUtilities.cpp.
// They are updated/managed by the library's existing
// BLE server implementation.

extern bool isDeviceConnected;
extern BLEServer *bleServer;


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

const int LEFT_PWM_CHANNEL = 0;
const int RIGHT_PWM_CHANNEL = 1;

bool pwmReady = false;


// =====================================================
// 4. TASK 6: RAMP CONFIGURATION
// =====================================================

// Required time to move from 0 to 255 PWM.

const float RAMP_DURATION = 1.5f;

// PWM slew rate.

const float PWM_SLEW_RATE =
    PWM_MAX / RAMP_DURATION;

// 170 PWM units per second.

// Limit a single update to 50 ms of ramping.

const float MAX_RAMP_INTERVAL = 0.050f;

unsigned long previousRampMicros = 0;


// =====================================================
// 5. INDEPENDENT MOTOR VARIABLES
// =====================================================

// LEFT MOTOR

float currentPWMLeft = 0.0f;

int targetPWMLeft = 0;


// RIGHT MOTOR

float currentPWMRight = 0.0f;

int targetPWMRight = 0;


// =====================================================
// 6. ADJUSTABLE SPEED CONTROL
// =====================================================

int speedCommandPWM = 0;

const int PWM_INCREMENT_LARGE = 10;

const int PWM_INCREMENT_SMALL = 1;


// =====================================================
// 7. BUTTON STATE VARIABLES
// =====================================================

bool previousSquare = false;

bool previousTriangle = false;

bool previousCircle = false;

bool previousCross = false;

bool previousSelect = false;


// =====================================================
// 8. EMERGENCY STOP VARIABLES
// =====================================================

bool emergencyStopLatched = false;


// =====================================================
// 9. BLUETOOTH CONNECTION VARIABLES
// =====================================================

// Tracks previous BLE connection state.

bool previousBLEConnection = false;

// Prevents automatic movement after reconnection.

bool motionArmed = false;

// Time when all directional buttons became neutral.

unsigned long neutralStartTime = 0;

const unsigned long NEUTRAL_REQUIRED_TIME = 250;


// Advertising restart after BLE disconnect.

bool advertisingRestartPending = false;

unsigned long disconnectTime = 0;

const unsigned long ADVERTISING_RESTART_DELAY = 500;


// =====================================================
// 10. PWM INITIALIZATION
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

    double leftReady = ledcSetup(
        LEFT_PWM_CHANNEL,
        PWM_FREQUENCY,
        PWM_RESOLUTION
    );

    double rightReady = ledcSetup(
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

    return leftReady > 0 && rightReady > 0;

#endif

}


// =====================================================
// 11. PWM OUTPUT FUNCTION
// =====================================================

void writeMotorPWM(
    int pwmPin,
    int pwmChannel,
    int duty
) {

    duty = constrain(
        duty,
        0,
        PWM_MAX
    );

#if ESP_ARDUINO_VERSION_MAJOR >= 3

    ledcWrite(pwmPin, duty);

#else

    ledcWrite(pwmChannel, duty);

#endif

}


// =====================================================
// 12. LEFT MOTOR OUTPUT
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
// 13. RIGHT MOTOR OUTPUT
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
// 14. IMMEDIATE MOTOR STOP
// =====================================================

// Used for emergency stops and Bluetooth loss.
// Bypasses normal PWM ramping.

void immediateMotorStop() {

    targetPWMLeft = 0;

    targetPWMRight = 0;

    currentPWMLeft = 0.0f;

    currentPWMRight = 0.0f;


    if (pwmReady) {

        writeMotorPWM(
            LEFT_PWM_PIN,
            LEFT_PWM_CHANNEL,
            0
        );

        writeMotorPWM(
            RIGHT_PWM_PIN,
            RIGHT_PWM_CHANNEL,
            0
        );

    }


    digitalWrite(LEFT_IN1, LOW);

    digitalWrite(LEFT_IN2, LOW);

    digitalWrite(RIGHT_IN1, LOW);

    digitalWrite(RIGHT_IN2, LOW);


    previousRampMicros = micros();

}


// =====================================================
// 15. ADJUSTABLE PWM CONTROL
// =====================================================

void updateSpeedControl() {

    bool square = GamePad.isSquarePressed();

    bool triangle = GamePad.isTrianglePressed();

    bool circle = GamePad.isCirclePressed();

    bool cross = GamePad.isCrossPressed();


    // Square: Increase PWM by 10

    if (square && !previousSquare) {

        speedCommandPWM += PWM_INCREMENT_LARGE;

    }

    // Triangle: Increase PWM by 1

    else if (triangle && !previousTriangle) {

        speedCommandPWM += PWM_INCREMENT_SMALL;

    }

    // Circle: Decrease PWM by 10

    else if (circle && !previousCircle) {

        speedCommandPWM -= PWM_INCREMENT_LARGE;

    }

    // Cross: Decrease PWM by 1

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
// 16. SYNCHRONIZE BUTTON STATES
// =====================================================

void synchronizeButtonStates() {

    previousSquare = GamePad.isSquarePressed();

    previousTriangle = GamePad.isTrianglePressed();

    previousCircle = GamePad.isCirclePressed();

    previousCross = GamePad.isCrossPressed();

    previousSelect = GamePad.isSelectPressed();

}


// =====================================================
// 17. EMERGENCY STOP
// =====================================================

void updateEmergencyStop() {

    bool startPressed = GamePad.isStartPressed();

    bool selectPressed = GamePad.isSelectPressed();


    // START activates emergency stop.

    if (startPressed && !emergencyStopLatched) {

        emergencyStopLatched = true;

        speedCommandPWM = 0;

        motionArmed = false;

        neutralStartTime = 0;

        immediateMotorStop();


        Serial.println();

        Serial.println(
            "!!! EMERGENCY STOP ACTIVATED !!!"
        );

    }


    // SELECT resets emergency stop.
    // START must be released first.

    else if (
        emergencyStopLatched &&
        selectPressed &&
        !previousSelect &&
        !startPressed
    ) {

        emergencyStopLatched = false;

        speedCommandPWM = 0;

        motionArmed = false;

        neutralStartTime = 0;


        Serial.println(
            "Emergency stop reset."
        );

        Serial.println(
            "Release directional controls before moving."
        );

    }


    previousSelect = selectPressed;

}


// =====================================================
// 18. CHECK DIRECTIONAL NEUTRAL STATE
// =====================================================

bool directionsReleased() {

    return
        !GamePad.isUpPressed() &&
        !GamePad.isDownPressed() &&
        !GamePad.isLeftPressed() &&
        !GamePad.isRightPressed();

}


// =====================================================
// 19. RECONNECTION ARMING SYSTEM
// =====================================================

void updateMotionArming() {

    if (motionArmed) {

        return;

    }


    // Motors must remain stopped while disarmed.

    targetPWMLeft = 0;

    targetPWMRight = 0;


    // Require all directional buttons to be released.

    if (directionsReleased()) {

        if (neutralStartTime == 0) {

            neutralStartTime = millis();

        }


        // Require a stable neutral state for 250 ms.

        if (
            millis() - neutralStartTime >=
            NEUTRAL_REQUIRED_TIME
        ) {

            motionArmed = true;

            synchronizeButtonStates();


            Serial.println();

            Serial.println(
                "Movement controls armed."
            );

            Serial.println(
                "A fresh direction command is required."
            );

        }

    }

    else {

        neutralStartTime = 0;

    }

}


// =====================================================
// 20. MOVEMENT TARGET CALCULATION
// =====================================================

void updateMovementTargets() {

    // Never allow movement while disarmed.

    if (!motionArmed || emergencyStopLatched) {

        targetPWMLeft = 0;

        targetPWMRight = 0;

        return;

    }


    bool up = GamePad.isUpPressed();

    bool down = GamePad.isDownPressed();

    bool left = GamePad.isLeftPressed();

    bool right = GamePad.isRightPressed();


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


    // NO DIRECTION BUTTON PRESSED

    // Releasing the GamePad gradually stops both motors.

    else {

        targetPWMLeft = 0;

        targetPWMRight = 0;

    }

}


// =====================================================
// 21. TASK 6: SLEW RATE LIMITER
// =====================================================

float rampPWM(
    float currentPWM,
    int targetPWM,
    float maximumChange
) {

    float effectiveTarget = (float)targetPWM;


    // Direction reversal protection.
    // First ramp the current PWM to zero.

    if (
        (currentPWM > 0.0f && targetPWM < 0) ||
        (currentPWM < 0.0f && targetPWM > 0)
    ) {

        effectiveTarget = 0.0f;

    }


    float difference =
        effectiveTarget - currentPWM;


    // Target reached.

    if (fabsf(difference) <= maximumChange) {

        return effectiveTarget;

    }


    // Increase PWM gradually.

    if (difference > 0.0f) {

        currentPWM += maximumChange;

    }


    // Decrease PWM gradually.

    else {

        currentPWM -= maximumChange;

    }


    return currentPWM;

}


// =====================================================
// 22. TASK 6: UPDATE BOTH MOTOR RAMPS
// =====================================================

void updateMotorRamp() {

    unsigned long now = micros();


    if (previousRampMicros == 0) {

        previousRampMicros = now;

        return;

    }


    // Actual elapsed time.

    unsigned long elapsedMicros =
        now - previousRampMicros;

    previousRampMicros = now;


    float elapsedTime =
        elapsedMicros / 1000000.0f;


    // Limit the maximum update interval.

    if (elapsedTime > MAX_RAMP_INTERVAL) {

        elapsedTime = MAX_RAMP_INTERVAL;

    }


    // Calculate permitted PWM change.

    float maximumChange =
        PWM_SLEW_RATE * elapsedTime;


    // LEFT MOTOR

    currentPWMLeft = rampPWM(
        currentPWMLeft,
        targetPWMLeft,
        maximumChange
    );


    // RIGHT MOTOR

    currentPWMRight = rampPWM(
        currentPWMRight,
        targetPWMRight,
        maximumChange
    );


    // Apply outputs independently.

    applyLeftMotorOutput();

    applyRightMotorOutput();

}


// =====================================================
// 23. TASK 8: BLUETOOTH CONNECTION MONITOR
// =====================================================

void updateBluetoothConnection() {

    bool connected = isDeviceConnected;


    // -----------------------------------------------
    // DISCONNECTION DETECTED
    // -----------------------------------------------

    if (previousBLEConnection && !connected) {

        Serial.println();

        Serial.println(
            "!!! BLUETOOTH CONNECTION LOST !!!"
        );


        // Immediately stop both motors.

        immediateMotorStop();


        // Clear active speed and movement commands.

        speedCommandPWM = 0;

        targetPWMLeft = 0;

        targetPWMRight = 0;


        // Require fresh input after reconnection.

        motionArmed = false;

        neutralStartTime = 0;


        // Reset button edge states.

        previousSquare = false;

        previousTriangle = false;

        previousCircle = false;

        previousCross = false;

        previousSelect = false;


        Serial.println(
            "Both motors stopped immediately."
        );

        Serial.println(
            "Waiting for Bluetooth reconnection..."
        );


        // Schedule advertising restart using the
        // existing Dabble BLE server.

        advertisingRestartPending = true;

        disconnectTime = millis();

    }


    // -----------------------------------------------
    // RECONNECTION DETECTED
    // -----------------------------------------------

    if (!previousBLEConnection && connected) {

        Serial.println();

        Serial.println(
            "Bluetooth reconnected."
        );

        Serial.println(
            "Waiting for neutral GamePad input..."
        );


        // Do not resume previous movement.

        immediateMotorStop();

        speedCommandPWM = 0;

        motionArmed = false;

        neutralStartTime = 0;


        advertisingRestartPending = false;

    }


    // Update connection history.

    previousBLEConnection = connected;


    // -----------------------------------------------
    // RESTART ADVERTISING AFTER DISCONNECTION
    // -----------------------------------------------

    // Reuse the server created by Dabble.begin().
    // No second BLE server is created.

    if (
        advertisingRestartPending &&
        !connected &&
        millis() - disconnectTime >=
        ADVERTISING_RESTART_DELAY
    ) {

        if (bleServer != nullptr) {

            bleServer->startAdvertising();

            Serial.println(
                "BLE advertising restarted."
            );

        }

        else {

            Serial.println(
                "WARNING: Dabble BLE server unavailable."
            );

        }


        advertisingRestartPending = false;

    }

}


// =====================================================
// 24. SERIAL MONITOR STATUS
// =====================================================

void printStatus() {

    static unsigned long previousPrint = 0;


    if (millis() - previousPrint < 500) {

        return;

    }

    previousPrint = millis();


    Serial.print("BLE: ");

    Serial.print(
        isDeviceConnected ? "CONNECTED" : "DISCONNECTED"
    );


    Serial.print(" | Speed: ");

    Serial.print(speedCommandPWM);


    Serial.print(" | Left Target: ");

    Serial.print(targetPWMLeft);


    Serial.print(" | Left Current: ");

    Serial.print(currentPWMLeft, 1);


    Serial.print(" | Right Target: ");

    Serial.print(targetPWMRight);


    Serial.print(" | Right Current: ");

    Serial.print(currentPWMRight, 1);


    Serial.print(" | Armed: ");

    Serial.print(motionArmed ? "YES" : "NO");


    Serial.print(" | E-STOP: ");

    Serial.println(
        emergencyStopLatched ? "ACTIVE" : "OFF"
    );

}


// =====================================================
// 25. SETUP
// =====================================================

void setup() {

    Serial.begin(115200);

    delay(500);


    Serial.println();

    Serial.println(
        "========================================"
    );

    Serial.println(
        "UGV PART 1 - COMBINED FIRMWARE"
    );

    Serial.println(
        "TASK 3 + TASK 4 + TASK 5 + TASK 6 + TASK 8"
    );

    Serial.println(
        "========================================"
    );


    // -----------------------------------------------
    // MOTOR DIRECTION PIN INITIALIZATION
    // -----------------------------------------------

    pinMode(LEFT_IN1, OUTPUT);

    pinMode(LEFT_IN2, OUTPUT);

    pinMode(RIGHT_IN1, OUTPUT);

    pinMode(RIGHT_IN2, OUTPUT);


    // Keep both motors stopped.

    digitalWrite(LEFT_IN1, LOW);

    digitalWrite(LEFT_IN2, LOW);

    digitalWrite(RIGHT_IN1, LOW);

    digitalWrite(RIGHT_IN2, LOW);


    // -----------------------------------------------
    // PWM INITIALIZATION
    // -----------------------------------------------

    Serial.println("Initializing motor PWM...");

    pwmReady = initializePWM();


    if (pwmReady) {

        writeMotorPWM(
            LEFT_PWM_PIN,
            LEFT_PWM_CHANNEL,
            0
        );

        writeMotorPWM(
            RIGHT_PWM_PIN,
            RIGHT_PWM_CHANNEL,
            0
        );

        Serial.println(
            "Motor PWM initialized successfully."
        );

    }

    else {

        Serial.println(
            "ERROR: PWM initialization failed."
        );

        Serial.println(
            "Motor operation disabled."
        );

    }


    // -----------------------------------------------
    // DABBLE BLE INITIALIZATION
    // -----------------------------------------------

    Serial.println("Initializing Dabble BLE...");

    Dabble.begin("UGV_ESP32");


    // -----------------------------------------------
    // INITIALIZE CONTROL VARIABLES
    // -----------------------------------------------

    speedCommandPWM = 0;

    currentPWMLeft = 0.0f;

    currentPWMRight = 0.0f;

    targetPWMLeft = 0;

    targetPWMRight = 0;

    previousRampMicros = micros();


    // Initial BLE state.

    previousBLEConnection = isDeviceConnected;


    Serial.println();

    Serial.println(
        "========================================"
    );

    Serial.println(
        "UGV CONTROLLER INITIALIZED"
    );

    Serial.println(
        "Bluetooth Name: UGV_ESP32"
    );

    Serial.println(
        "Initial PWM: 0"
    );

    Serial.println(
        "Maximum PWM: 255"
    );

    Serial.println(
        "Acceleration time: 1.5 seconds"
    );

    Serial.println(
        "Deceleration time: 1.5 seconds"
    );

    Serial.println();

    Serial.println(
        "SQUARE   : PWM +10"
    );

    Serial.println(
        "TRIANGLE : PWM +1"
    );

    Serial.println(
        "CIRCLE   : PWM -10"
    );

    Serial.println(
        "CROSS    : PWM -1"
    );

    Serial.println();

    Serial.println(
        "UP       : FORWARD"
    );

    Serial.println(
        "DOWN     : BACKWARD"
    );

    Serial.println(
        "LEFT     : LEFT PIVOT"
    );

    Serial.println(
        "RIGHT    : RIGHT PIVOT"
    );

    Serial.println();

    Serial.println(
        "START    : EMERGENCY STOP"
    );

    Serial.println(
        "SELECT   : RESET EMERGENCY STOP"
    );

    Serial.println(
        "========================================"
    );

}


// =====================================================
// 26. MAIN LOOP
// =====================================================

void loop() {

    // -----------------------------------------------
    // 1. Process incoming Dabble data.
    // -----------------------------------------------

    Dabble.processInput();


    // -----------------------------------------------
    // 2. Monitor BLE connection state.
    // -----------------------------------------------

    updateBluetoothConnection();


    // -----------------------------------------------
    // 3. Do not operate without BLE or valid PWM.
    // -----------------------------------------------

    if (!pwmReady || !isDeviceConnected) {

        return;

    }


    // -----------------------------------------------
    // 4. Process emergency stop.
    // -----------------------------------------------

    updateEmergencyStop();


    if (emergencyStopLatched) {

        immediateMotorStop();

        printStatus();

        return;

    }


    // -----------------------------------------------
    // 5. Require neutral input after startup/reconnect.
    // -----------------------------------------------

    updateMotionArming();


    // -----------------------------------------------
    // 6. Process speed and movement commands.
    // -----------------------------------------------

    if (motionArmed) {

        updateSpeedControl();

        updateMovementTargets();

    }

    else {

        targetPWMLeft = 0;

        targetPWMRight = 0;

    }


    // -----------------------------------------------
    // 7. Apply time-based smooth PWM ramping.
    // -----------------------------------------------

    updateMotorRamp();


    // -----------------------------------------------
    // 8. Display system status.
    // -----------------------------------------------

    printStatus();

}