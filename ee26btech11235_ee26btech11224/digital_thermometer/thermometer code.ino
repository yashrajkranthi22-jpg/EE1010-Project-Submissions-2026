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
const uint8_t SENSOR_PIN_A0 = A0;
const uint8_t SENSOR_PIN_A1 = A1;
const uint8_t UNIT_BUTTON_PIN = 8;

// ============================================================
// PRELIMINARY DIFFERENCE-BASED CALIBRATION
// feature = A0_mean - A1_mean
// T_C = C0 + C1 * ((feature - FEATURE_CENTER) / FEATURE_SCALE)
//
// Based on 16 approximate group means: ten earlier current-layout
// groups, five newer groups, and the ~29.7 C normal-position point.
// These are provisional coefficients: use the fixed
// final enclosure/wiring and validate on new sessions.
// ============================================================
const float FEATURE_CENTER = -214.0f;
const float FEATURE_SCALE  = 20.0f;
const float C0 = 60.934888f;
const float C1 = 26.589821f;

// Observed feature span of the calibration groups, with a small
// guard margin. This is not permission to extrapolate outside it.
const float FEATURE_MIN = -237.70f;
const float FEATURE_MAX = -194.80f;

// ============================================================
// MEASUREMENT AND DISPLAY SETTINGS
// ============================================================
const uint8_t SAMPLES = 64;
const unsigned long UPDATE_INTERVAL_MS = 1000;
const unsigned long DEBOUNCE_MS = 40;

bool showFahrenheit = false;
unsigned long lastUpdate = 0;

int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// ============================================================
// READ BOTH DIVIDERS
// Keep the circuit, wiring, excitation, and ADC reference fixed.
// The difference feature rejects much of the common additive shift
// seen when moving the perfboard; the ratio did not.
// ============================================================
bool readSensors(float &a0Mean, float &a1Mean) {
  digitalWrite(EXCITATION_PIN, HIGH);
  delay(5);

  // Discard initial conversions after excitation / channel start.
  (void)analogRead(SENSOR_PIN_A0);
  (void)analogRead(SENSOR_PIN_A1);

  unsigned long sumA0 = 0;
  unsigned long sumA1 = 0;

  for (uint8_t i = 0; i < SAMPLES; i++) {
    sumA0 += analogRead(SENSOR_PIN_A0);
    sumA1 += analogRead(SENSOR_PIN_A1);
  }

  digitalWrite(EXCITATION_PIN, LOW);

  a0Mean = (float)sumA0 / (float)SAMPLES;
  a1Mean = (float)sumA1 / (float)SAMPLES;

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
// DISPLAY TEMPERATURE AND FEATURE-RANGE STATUS
// ============================================================
void showOnLCD(float temperatureC, float feature, bool outOfCal) {
  float shownTemperature = temperatureC;
  if (showFahrenheit) {
    shownTemperature = temperatureC * 9.0f / 5.0f + 32.0f;
  }
  char unit = showFahrenheit ? 'F' : 'C';

  char tempBuffer[10];
  char featureBuffer[10];
  dtostrf(shownTemperature, 5, 1, tempBuffer);
  dtostrf(feature, 6, 1, featureBuffer);

  lcd.setCursor(0, 0);
  lcd.print(F("Temp:"));
  lcd.print(tempBuffer);
  lcd.print(' ');
  lcd.print(unit);
  lcd.print(F("   "));

  lcd.setCursor(0, 1);
  if (outOfCal) {
    lcd.print(F("OUT CAL D:"));
  } else {
    lcd.print(F("IN CAL  D:"));
  }
  lcd.print(featureBuffer);
  lcd.print(F(" "));
}

void showSensorError() {
  lcd.setCursor(0, 0);
  lcd.print(F("CHECK SENSOR    "));
  lcd.setCursor(0, 1);
  lcd.print(F("Check A0/A1/GND "));
}

// ============================================================
// SERIAL DIAGNOSTICS
// Open Serial Monitor at 115200 baud.
// ============================================================
void printToSerial(float a0Mean, float a1Mean, float difference,
                   float ratio, float temperatureC, bool outOfCal) {
  float shownTemperature = temperatureC;
  if (showFahrenheit) {
    shownTemperature = temperatureC * 9.0f / 5.0f + 32.0f;
  }

  Serial.print(F("A0_mean=")); Serial.print(a0Mean, 3);
  Serial.print(F(", A1_mean=")); Serial.print(a1Mean, 3);
  Serial.print(F(", diff_count=")); Serial.print(difference, 3);
  Serial.print(F(", ratio_diagnostic=")); Serial.print(ratio, 6);
  Serial.print(F(", Tfit_C=")); Serial.print(temperatureC, 3);
  Serial.print(F(", Temperature=")); Serial.print(shownTemperature, 1);
  Serial.print(' '); Serial.print(showFahrenheit ? 'F' : 'C');
  Serial.print(F(", status="));
  Serial.println(outOfCal ? F("OUT_OF_CAL_ESTIMATE") : F("IN_CAL"));
}

void setup() {
  pinMode(EXCITATION_PIN, OUTPUT);
  digitalWrite(EXCITATION_PIN, LOW);
  pinMode(UNIT_BUTTON_PIN, INPUT_PULLUP);

  // Uno R3 internal nominal 1.1 V reference. Its actual voltage
  // need not be exactly 1.100 V; the ratio helps cancel a common
  // ADC scale factor when both channels are measured consistently.
  analogReference(INTERNAL);
  delay(100);
  (void)analogRead(SENSOR_PIN_A0);
  (void)analogRead(SENSOR_PIN_A1);

  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); lcd.print(F("PT100 Thermo"));
  lcd.setCursor(0, 1); lcd.print(F("Diff calibration"));
  delay(800);
  lcd.clear();

  lastButtonReading = digitalRead(UNIT_BUTTON_PIN);
  stableButtonState = lastButtonReading;
  lastUpdate = millis();
}

void loop() {
  checkUnitButton();
  if (millis() - lastUpdate < UPDATE_INTERVAL_MS) return;
  lastUpdate = millis();

  float a0Mean = 0.0f;
  float a1Mean = 0.0f;
  if (!readSensors(a0Mean, a1Mean)) {
    showSensorError();
    Serial.println(F("ERROR: Invalid ADC reading; check divider wiring."));
    return;
  }

  float difference = a0Mean - a1Mean;
  float ratio = a0Mean / a1Mean; // diagnostic only; not the calibration feature
  float z = (difference - FEATURE_CENTER) / FEATURE_SCALE;
  float temperatureC = C0 + C1 * z;
  bool outOfCal = (difference < FEATURE_MIN || difference > FEATURE_MAX);

  // No extra exponential temperature filter is applied. Averaging
  // 64 pairs already reduces ADC noise; avoid a second filter
  // during calibration and independent validation.
  showOnLCD(temperatureC, difference, outOfCal);
  printToSerial(a0Mean, a1Mean, difference, ratio, temperatureC, outOfCal);
}
