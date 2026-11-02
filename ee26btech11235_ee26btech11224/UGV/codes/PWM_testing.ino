/*
====================================================
UGV PART 1 - TASK 3
INDEPENDENT PWM SPEED CONTROL

Microcontroller : ESP32 DevKit V1
Motor Driver    : L293D
Motors          : Two DC Geared Motors

PWM Frequency   : 2000 Hz
PWM Resolution  : 8 bits
PWM Range       : 0 - 255

ESP32 Arduino Core: 3.x
====================================================
*/


// ================================================
// 1. GPIO CONFIGURATION
// ================================================

// Left motor
const int LEFT_PWM = 18;
const int LEFT_IN1 = 32;
const int LEFT_IN2 = 33;

// Right motor
const int RIGHT_PWM = 19;
const int RIGHT_IN1 = 25;
const int RIGHT_IN2 = 26;


// ================================================
// 2. PWM CONFIGURATION
// ================================================

const int PWM_FREQUENCY = 2000;
const int PWM_RESOLUTION = 8;
const int PWM_MAX = 255;


// ================================================
// 3. MOTOR DIRECTION CONFIGURATION
// ================================================

// Change these to true if the corresponding motor's
// logical forward direction needs to be reversed.

const bool LEFT_DIRECTION_INVERTED = false;
const bool RIGHT_DIRECTION_INVERTED = false;


// ================================================
// 4. INITIALIZE MOTORS
// ================================================

void setupMotors() {

  // Configure direction pins as outputs

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);


  // Initially keep direction inputs LOW

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);


  // Configure independent PWM outputs

  bool leftStatus = ledcAttach(
    LEFT_PWM,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );

  bool rightStatus = ledcAttach(
    RIGHT_PWM,
    PWM_FREQUENCY,
    PWM_RESOLUTION
  );


  // Check PWM initialization

  if (!leftStatus || !rightStatus) {

    Serial.println("ERROR: PWM initialization failed!");

    while (true) {
      delay(1000);
    }

  }


  // Initially stop both motors

  ledcWrite(LEFT_PWM, 0);
  ledcWrite(RIGHT_PWM, 0);

  Serial.println("PWM initialization successful!");

}


// ================================================
// 5. GENERAL MOTOR CONTROL FUNCTION
// ================================================

void setMotor(
  int pwmPin,
  int in1,
  int in2,
  int speed,
  char direction,
  bool inverted
) {

  // Restrict PWM to 0-255

  speed = constrain(speed, 0, PWM_MAX);


  // Determine electrical direction

  bool forward = (direction == 'F');

  bool electricalForward = forward ^ inverted;


  // Disable motor before changing direction

  ledcWrite(pwmPin, 0);

  delayMicroseconds(100);


  // Set direction

  if (electricalForward) {

    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

  }

  else {

    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

  }


  // Apply PWM speed

  ledcWrite(pwmPin, speed);

}


// ================================================
// 6. LEFT MOTOR CONTROL
// ================================================

void setLeftMotor(int speed, char direction) {

  setMotor(
    LEFT_PWM,
    LEFT_IN1,
    LEFT_IN2,
    speed,
    direction,
    LEFT_DIRECTION_INVERTED
  );

}


// ================================================
// 7. RIGHT MOTOR CONTROL
// ================================================

void setRightMotor(int speed, char direction) {

  setMotor(
    RIGHT_PWM,
    RIGHT_IN1,
    RIGHT_IN2,
    speed,
    direction,
    RIGHT_DIRECTION_INVERTED
  );

}


// ================================================
// 8. STOP BOTH MOTORS
// ================================================

void stopMotors() {

  // Disable both motor drivers

  ledcWrite(LEFT_PWM, 0);
  ledcWrite(RIGHT_PWM, 0);


  // Set direction inputs LOW

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);


  Serial.println("Both motors stopped.");

}


// ================================================
// 9. PROCESS SERIAL COMMANDS
// ================================================

void processSerialCommand() {

  if (!Serial.available()) {
    return;
  }


  // Read command from Serial Monitor

  String command = Serial.readStringUntil('\n');

  command.trim();


  // Stop command

  if (command.equalsIgnoreCase("S")) {

    stopMotors();

    return;

  }


  // Variables for motor commands

  int leftSpeed;
  int rightSpeed;

  char leftDirection;
  char rightDirection;


  // Expected format:
  // leftSpeed rightSpeed leftDirection rightDirection
  // Example: 100 160 F F

  int fields = sscanf(
    command.c_str(),
    "%d %d %c %c",
    &leftSpeed,
    &rightSpeed,
    &leftDirection,
    &rightDirection
  );


  // Validate number of inputs

  if (fields != 4) {

    Serial.println("Invalid command!");

    Serial.println("Format: LEFT RIGHT LEFT_DIR RIGHT_DIR");

    Serial.println("Example: 100 160 F F");

    return;

  }


  // Convert lowercase directions to uppercase

  if (leftDirection == 'f') leftDirection = 'F';
  if (leftDirection == 'r') leftDirection = 'R';

  if (rightDirection == 'f') rightDirection = 'F';
  if (rightDirection == 'r') rightDirection = 'R';


  // Validate PWM values

  if (
    leftSpeed < 0 || leftSpeed > PWM_MAX ||
    rightSpeed < 0 || rightSpeed > PWM_MAX
  ) {

    Serial.println("ERROR: PWM must be between 0 and 255.");

    return;

  }


  // Validate directions

  if (
    (leftDirection != 'F' && leftDirection != 'R') ||
    (rightDirection != 'F' && rightDirection != 'R')
  ) {

    Serial.println("ERROR: Direction must be F or R.");

    return;

  }


  // Apply independent motor commands

  setLeftMotor(leftSpeed, leftDirection);

  setRightMotor(rightSpeed, rightDirection);


  // Display motor settings

  Serial.println("-----------------------------");

  Serial.print("Left PWM: ");
  Serial.println(leftSpeed);

  Serial.print("Right PWM: ");
  Serial.println(rightSpeed);

  Serial.print("Left Direction: ");
  Serial.println(leftDirection);

  Serial.print("Right Direction: ");
  Serial.println(rightDirection);

  Serial.println("-----------------------------");

}


// ================================================
// 10. SETUP
// ================================================

void setup() {

  Serial.begin(115200);

  Serial.setTimeout(100);

  delay(500);

  Serial.println();
  Serial.println("================================");
  Serial.println("UGV - INDEPENDENT PWM CONTROL");
  Serial.println("================================");


  // Initialize motor driver

  setupMotors();


  // Display instructions

  Serial.println();
  Serial.println("SERIAL COMMAND FORMAT:");

  Serial.println("LEFT_PWM RIGHT_PWM LEFT_DIR RIGHT_DIR");

  Serial.println();

  Serial.println("Examples:");

  Serial.println("100 160 F F");

  Serial.println("100 0 F F");

  Serial.println("0 100 F F");

  Serial.println("150 150 R R");

  Serial.println("S = STOP BOTH MOTORS");

  Serial.println();

  Serial.println("Enter a command:");

}


// ================================================
// 11. MAIN LOOP
// ================================================

void loop() {

  processSerialCommand();

}