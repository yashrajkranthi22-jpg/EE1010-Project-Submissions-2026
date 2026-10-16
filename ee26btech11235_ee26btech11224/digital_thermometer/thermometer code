
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdlib.h>
#include <math.h>

// ============================================================
// LCD: JHD162A-compatible 16x2 LCD in 4-bit mode
// RS -> D12, E -> D11
// D4 -> D5, D5 -> D4, D6 -> D3, D7 -> D2
// ============================================================
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ============================================================
// HARDWARE PINS
// ============================================================
const uint8_t EXCITATION_PIN = 7;
const uint8_t SENSOR_PIN_A0  = A0;
const uint8_t SENSOR_PIN_A1  = A1;
const uint8_t UNIT_BUTTON_PIN = 8;

// ============================================================
// LEAST-SQUARES CALIBRATION
// Keep these coefficients unchanged for your current model.
// d = A0_mean - A1_mean
// ============================================================
const float FEATURE_CENTER = -208.86618f;
const float FEATURE_SCALE  = 8.71028423f;
const float C0 = 54.75f;
const float C1 = 12.1019821f;

// Calibration feature range from the existing dataset
const float FEATURE_MIN = -221.46f;
const float FEATURE_MAX = -192.73f;

// ============================================================
// MEASUREMENT AND DISPLAY SETTINGS
// ============================================================
const uint8_t SAMPLES = 64;
const unsigned long UPDATE_INTERVAL_MS = 1000;
const unsigned long DEBOUNCE_MS = 40;

// The model always calculates Celsius internally.
bool showFahrenheit = false;

unsigned long lastUpdate = 0;

// Button debounce state
int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// Temperature filtering state
bool haveFilteredTemperature = false;
float filteredTemperatureC = 0.0f;

// ============================================================
// READ THE TWO ANALOGUE CHANNELS
// D7 drives the existing sensor circuit during measurement.
// ============================================================
bool readSensors(float &a0Mean, float &a1Mean) {
  digitalWrite(EXCITATION_PIN, HIGH);
  delay(5);

  // Discard initial conversions after channel/reference changes.
  (void)analogRead(SENSOR_PIN_A0);
  (void)analogRead(SENSOR_PIN_A1);

  unsigned long sumA0 = 0;
  unsigned long sumA1 = 0;

  for (uint8_t i = 0; i < SAMPLES; i++) {
    sumA0 += analogRead(SENSOR_PIN_A0);
    sumA1 += analogRead(SENSOR_PIN_A1);
  }

  digitalWrite(EXCITATION_PIN, LOW);

  a0Mean = (float)sumA0 / SAMPLES;
  a1Mean = (float)sumA1 / SAMPLES;

  // Reject readings at the ADC extremes.
  if (a0Mean <= 1.0f || a0Mean >= 1022.0f ||
      a1Mean <= 1.0f || a1Mean >= 1022.0f) {
    return false;
  }

  return true;
}

// ============================================================
// CHECK AND DEBOUNCE THE CELSIUS/FAHRENHEIT BUTTON
// Button connects D8 to GND when pressed.
// ============================================================
void checkUnitButton() {
  int reading = digitalRead(UNIT_BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
    lastButtonReading = reading;
  }

  if (millis() - lastDebounceTime >= DEBOUNCE_MS) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      if (stableButtonState == LOW) {
        showFahrenheit = !showFahrenheit;
      }
    }
  }
}

// ============================================================
// DISPLAY THE TEMPERATURE AND CALIBRATION STATUS
// Uses dtostrf() because AVR snprintf() commonly does not
// support floating-point %f formatting by default.
// ============================================================
void showOnLCD(float temperatureC, float feature, bool outOfCal) {
  float shownTemperature;

  if (showFahrenheit) {
    shownTemperature = temperatureC * 9.0f / 5.0f + 32.0f;
  } else {
    shownTemperature = temperatureC;
  }

  char unit = showFahrenheit ? 'F' : 'C';

  char tempBuffer[10];
  char featureBuffer[10];

  dtostrf(shownTemperature, 5, 1, tempBuffer);
  dtostrf(feature, 6, 1, featureBuffer);

  // Row 1: temperature.
  lcd.setCursor(0, 0);
  lcd.print(F("Temp:"));
  lcd.print(tempBuffer);
  lcd.print(' ');
  lcd.print(unit);
  lcd.print(F("    "));

  // Row 2: calibration-range status and measured feature.
  lcd.setCursor(0, 1);

  if (outOfCal) {
    lcd.print(F("OUT CAL D:"));
  } else {
    lcd.print(F("IN CAL  D:"));
  }

  lcd.print(featureBuffer);
}

// ============================================================
// DISPLAY SENSOR ERROR
// ============================================================
void showSensorError() {
  lcd.setCursor(0, 0);
  lcd.print(F("CHECK SENSOR    "));

  lcd.setCursor(0, 1);
  lcd.print(F("Check wiring    "));
}

// ============================================================
// SERIAL DIAGNOSTICS
// Open Serial Monitor at 115200 baud.
// ============================================================
void printToSerial(
  float a0Mean,
  float a1Mean,
  float feature,
  float fittedTemperatureC,
  float displayTemperatureC,
  bool outOfCal
) {
  float shownTemperature;

  if (showFahrenheit) {
    shownTemperature =
      displayTemperatureC * 9.0f / 5.0f + 32.0f;
  } else {
    shownTemperature = displayTemperatureC;
  }

  char unit = showFahrenheit ? 'F' : 'C';

  Serial.print(F("A0_mean="));
  Serial.print(a0Mean, 3);

  Serial.print(F(", A1_mean="));
  Serial.print(a1Mean, 3);

  Serial.print(F(", feature="));
  Serial.print(feature, 3);

  Serial.print(F(", Tfit_C="));
  Serial.print(fittedTemperatureC, 3);

  Serial.print(F(", Tdisplay_C="));
  Serial.print(displayTemperatureC, 3);

  Serial.print(F(", Temperature="));
  Serial.print(shownTemperature, 1);
  Serial.print(' ');
  Serial.print(unit);

  Serial.print(F(", status="));

  if (outOfCal) {
    Serial.println(F("OUT_OF_CAL_ESTIMATE"));
  } else {
    Serial.println(F("IN_CAL"));
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  pinMode(EXCITATION_PIN, OUTPUT);
  digitalWrite(EXCITATION_PIN, LOW);

  pinMode(UNIT_BUTTON_PIN, INPUT_PULLUP);

  // Use the Uno R3's internal nominal 1.1 V ADC reference.
  analogReference(INTERNAL);
  delay(50);

  // Discard startup conversions.
  (void)analogRead(SENSOR_PIN_A0);
  (void)analogRead(SENSOR_PIN_A1);

  Serial.begin(115200);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(F("PT100 Thermo"));
  lcd.setCursor(0, 1);
  lcd.print(F("Starting..."));
  delay(700);
  lcd.clear();

  lastButtonReading = digitalRead(UNIT_BUTTON_PIN);
  stableButtonState = lastButtonReading;

  lastUpdate = millis();
}

// ============================================================
// MAIN LOOP
// ============================================================
void loop() {
  checkUnitButton();

  if (millis() - lastUpdate < UPDATE_INTERVAL_MS) {
    return;
  }

  lastUpdate = millis();

  float a0Mean = 0.0f;
  float a1Mean = 0.0f;

  if (!readSensors(a0Mean, a1Mean)) {
    showSensorError();
    Serial.println(F("ERROR: Invalid ADC reading. Check sensor wiring."));
    return;
  }

  // Calculate the calibrated feature.
  float feature = a0Mean - a1Mean;

  // Least-squares calibration model.
  float z = (feature - FEATURE_CENTER) / FEATURE_SCALE;
  float fittedTemperatureC = C0 + C1 * z;

  // Smooth small changes; accept larger changes immediately.
  if (!haveFilteredTemperature) {
    filteredTemperatureC = fittedTemperatureC;
    haveFilteredTemperature = true;
  } else if (fabsf(fittedTemperatureC - filteredTemperatureC) >= 1.5f) {
    filteredTemperatureC = fittedTemperatureC;
  } else {
    filteredTemperatureC =
      0.65f * filteredTemperatureC +
      0.35f * fittedTemperatureC;
  }

  // Do not hide the estimate outside the calibration range.
  // Instead, explicitly mark it as an extrapolated estimate.
  bool outOfCal =
    (feature < FEATURE_MIN || feature > FEATURE_MAX);

  showOnLCD(filteredTemperatureC, feature, outOfCal);

  printToSerial(
    a0Mean,
    a1Mean,
    feature,
    fittedTemperatureC,
    filteredTemperatureC,
    outOfCal
  );
}
