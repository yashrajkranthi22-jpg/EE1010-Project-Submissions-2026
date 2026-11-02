#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

#include <DabbleESP32.h>

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);

  Dabble.begin("UGV_ESP32");

  Serial.println("UGV Bluetooth Connected");
  Serial.println("Waiting for GamePad input...");
}

void loop() {

  Dabble.processInput();

  if (millis() - lastPrint >= 2000) {

    lastPrint = millis();

    Serial.print("Buttons: ");

    bool pressed = false;

    if (GamePad.isUpPressed()) {
      Serial.print("UP ");
      pressed = true;
    }

    if (GamePad.isDownPressed()) {
      Serial.print("DOWN ");
      pressed = true;
    }

    if (GamePad.isLeftPressed()) {
      Serial.print("LEFT ");
      pressed = true;
    }

    if (GamePad.isRightPressed()) {
      Serial.print("RIGHT ");
      pressed = true;
    }

    if (GamePad.isSquarePressed()) {
      Serial.print("SQUARE ");
      pressed = true;
    }

    if (GamePad.isCirclePressed()) {
      Serial.print("CIRCLE ");
      pressed = true;
    }

    if (!pressed) {
      Serial.print("NONE");
    }

    Serial.print(" | X: ");
    Serial.print(GamePad.getXaxisData());

    Serial.print(" | Y: ");
    Serial.println(GamePad.getYaxisData());
  }
}