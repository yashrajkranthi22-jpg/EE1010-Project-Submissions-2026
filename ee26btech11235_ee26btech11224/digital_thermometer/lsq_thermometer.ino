
/*
  PT100 Differential ADC Logger
  Board: Classic Arduino Uno R3 (ATmega328P)

  Wiring:
  D7  -> Upper end of both 2.2 kΩ resistors
  A0  -> Junction of first 2.2 kΩ resistor and PT100
  A1  -> Junction of second 2.2 kΩ resistor and 240 Ω resistor
  GND -> Other end of PT100 and other end of 240 Ω resistor

  LCD backlight circuit stays separate.

  Serial output:
  time_ms,A0_mean,A1_mean,d_count

  d_count = A0_mean - A1_mean
*/

const byte EXCITATION_PIN = 7;
const byte SENSOR_PIN = A0;
const byte REFERENCE_PIN = A1;

const int SAMPLES = 64;

void setup() {
  pinMode(EXCITATION_PIN, OUTPUT);
  digitalWrite(EXCITATION_PIN, LOW);

  Serial.begin(9600);

  // Use the nominal 1.1 V internal reference
  // on a classic Uno R3.
  analogReference(INTERNAL);

  // Allow the reference to settle.
  delay(100);

  // Discard initial ADC conversions.
  analogRead(SENSOR_PIN);
  analogRead(REFERENCE_PIN);

  Serial.println("time_ms,A0_mean,A1_mean,d_count");
}

void loop() {
  unsigned long sumA0 = 0;
  unsigned long sumA1 = 0;

  // Enable bridge excitation.
  digitalWrite(EXCITATION_PIN, HIGH);
  delay(5);

  // Discard the first conversions after excitation.
  analogRead(SENSOR_PIN);
  analogRead(REFERENCE_PIN);

  // Collect 64 readings from each midpoint.
  for (int i = 0; i < SAMPLES; i++) {
    sumA0 += analogRead(SENSOR_PIN);
    sumA1 += analogRead(REFERENCE_PIN);
  }

  // Disable excitation between measurements.
  digitalWrite(EXCITATION_PIN, LOW);

  // Calculate average ADC counts.
  float A0_mean = sumA0 / (float)SAMPLES;
  float A1_mean = sumA1 / (float)SAMPLES;

  // Calculate the differential measurement.
  float d_count = A0_mean - A1_mean;

  // Print comma-separated values.
  Serial.print(millis());
  Serial.print(",");
  Serial.print(A0_mean, 2);
  Serial.print(",");
  Serial.print(A1_mean, 2);
  Serial.print(",");
  Serial.println(d_count, 2);

  delay(850);
}
