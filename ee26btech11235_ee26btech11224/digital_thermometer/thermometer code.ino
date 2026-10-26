#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdlib.h>
#include <math.h>
#include "pt100_quadratic_coefficients.h"

// ============================================================
// PT100 thermometer: quadratic least-squares calibration
//
// Calibration feature:
//   d = A0_mean - A1_mean
// Normalized feature:
//   z = (d - FEATURE_CENTER) / FEATURE_SCALE
// Quadratic LSQ model:
//   T_C = LSQ_C0 + LSQ_C1*z + LSQ_C2*z*z
//
// IMPORTANT:
// This version ALWAYS displays the model's numerical temperature
// estimate after sampling A0 and A1. There is no A1 layout guard
// and no ADC-rail check that blocks the display.
// Instead, the second LCD row and Serial status identify ADC-rail
// readings or features outside the calibration range.
//
// A displayed estimate outside the calibrated feature range is an
// extrapolation, not a claim of accuracy. Fix hardware instability
// even though the display continues to show the calculated estimate.
// ============================================================

// LCD: RS -> D12, E -> D11; LCD D4..D7 -> Arduino D5,D4,D3,D2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Hardware pins
const uint8_t EXCITATION_PIN = 7;
const uint8_t SENSOR_PIN_A0 = A0;
const uint8_t SENSOR_PIN_A1 = A1;
const uint8_t UNIT_BUTTON_PIN = 8;

// Calibration feature envelope from the grouped calibration data.
// This only labels the estimate; it never prevents displaying it.
const float FEATURE_MIN = -237.80f;
const float FEATURE_MAX = -194.80f;

// Measurement and display settings
const uint8_t SAMPLES = 64;
const unsigned long UPDATE_INTERVAL_MS = 1000;
const unsigned long DEBOUNCE_MS = 40;

bool showFahrenheit = false;
unsigned long lastUpdate = 0;

int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// ============================================================
// READ BOTH ANALOGUE CHANNELS
// Readings at the ADC rails are still returned to the caller.
// The caller computes and displays an estimate, then labels the
// measurement as an ADC-rail estimate if applicable.
// ============================================================
void readSensors(float &a0Mean, float &a1Mean) {
  digitalWrite(EXCITATION_PIN, HIGH);
  delay(5);

  // Discard initial conversions after starting measurement.
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
}

// ============================================================
// C/F BUTTON
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
// DISPLAY
// The temperature is displayed regardless of ADC means or range.
// The second row warns when data are outside normal conditions.
// ============================================================
void showOnLCD(float temperatureC, float difference,
               bool outOfCal, bool adcRail) {
  float shownTemperature = temperatureC;
  if (showFahrenheit) {
    shownTemperature = temperatureC * 9.0f / 5.0f + 32.0f;
  }
  char unit = showFahrenheit ? 'F' : 'C';

  char tempBuffer[12];
  char featureBuffer[12];
  dtostrf(shownTemperature, 5, 1, tempBuffer);
  dtostrf(difference, 6, 1, featureBuffer);

  // Row 1: always show the calculated estimate.
  lcd.setCursor(0, 0);
  lcd.print(F("Temp:"));
  lcd.print(tempBuffer);
  lcd.print(' ');
  lcd.print(unit);
  lcd.print(F("    "));

  // Row 2: status only; do not block the temperature display.
  lcd.setCursor(0, 1);
  if (adcRail) {
    lcd.print(F("ADC RAIL D="));
    char shortFeature[8];
    dtostrf(difference, 5, 0, shortFeature);
    lcd.print(shortFeature);
  } else if (outOfCal) {
    lcd.print(F("OUT CAL D="));
    lcd.print(featureBuffer);
  } else {
    lcd.print(F("IN CAL D="));
    lcd.print(featureBuffer);
    lcd.print(' ');
  }
}

// ============================================================
// SERIAL DIAGNOSTICS
// Open Serial Monitor at 115200 baud.
// ============================================================
void printMeasurements(float a0Mean, float a1Mean,
                       float difference, bool ratioAvailable,
                       float ratio) {
  Serial.print(F("A0_mean=")); Serial.print(a0Mean, 3);
  Serial.print(F(", A1_mean=")); Serial.print(a1Mean, 3);
  Serial.print(F(", diff_count=")); Serial.print(difference, 3);
  Serial.print(F(", ratio_diagnostic="));
  if (ratioAvailable) {
    Serial.print(ratio, 6);
  } else {
    Serial.print(F("NA"));
  }
}

void printTemperature(float a0Mean, float a1Mean, float difference,
                      bool ratioAvailable, float ratio, float z,
                      float temperatureC, bool outOfCal, bool adcRail) {
  float shownTemperature = temperatureC;
  if (showFahrenheit) {
    shownTemperature = temperatureC * 9.0f / 5.0f + 32.0f;
  }

  printMeasurements(a0Mean, a1Mean, difference, ratioAvailable, ratio);
  Serial.print(F(", z=")); Serial.print(z, 6);
  Serial.print(F(", Tfit_C=")); Serial.print(temperatureC, 3);
  Serial.print(F(", Temperature=")); Serial.print(shownTemperature, 1);
  Serial.print(' '); Serial.print(showFahrenheit ? 'F' : 'C');
  Serial.print(F(", status="));

  if (adcRail) {
    Serial.println(F("ADC_RAIL_ESTIMATE"));
  } else if (outOfCal) {
    Serial.println(F("OUT_OF_CAL_ESTIMATE"));
  } else {
    Serial.println(F("IN_CAL"));
  }
}

void setup() {
  pinMode(EXCITATION_PIN, OUTPUT);
  digitalWrite(EXCITATION_PIN, LOW);
  pinMode(UNIT_BUTTON_PIN, INPUT_PULLUP);

  // Arduino Uno R3 nominal internal 1.1 V ADC reference.
  // The actual reference value is not assumed to be exactly 1.100 V.
  analogReference(INTERNAL);
  delay(100);
  (void)analogRead(SENSOR_PIN_A0);
  (void)analogRead(SENSOR_PIN_A1);

  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); lcd.print(F("PT100 Thermo"));
  lcd.setCursor(0, 1); lcd.print(F("Quadratic LSQ"));
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
  readSensors(a0Mean, a1Mean);

  const float difference = a0Mean - a1Mean;

  // Ratio is diagnostic only; protect against division by zero.
  const bool ratioAvailable = (a1Mean > 0.0f);
  const float ratio = ratioAvailable ? (a0Mean / a1Mean) : 0.0f;

  // A rail reading is flagged, but DOES NOT block displaying a temperature.
  const bool adcRail =
      (a0Mean <= 1.0f || a0Mean >= 1022.0f ||
       a1Mean <= 1.0f || a1Mean >= 1022.0f);

  const float z = (difference - FEATURE_CENTER) / FEATURE_SCALE;
  const float temperatureC = LSQ_C0 + LSQ_C1 * z + LSQ_C2 * z * z;
  const bool outOfCal =
      (difference < FEATURE_MIN || difference > FEATURE_MAX);

  showOnLCD(temperatureC, difference, outOfCal, adcRail);
  printTemperature(a0Mean, a1Mean, difference, ratioAvailable, ratio,
                   z, temperatureC, outOfCal, adcRail);
}
