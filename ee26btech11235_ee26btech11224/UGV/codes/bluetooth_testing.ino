#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

#include <DabbleESP32.h>

void setup() {

  Serial.begin(115200);

  // Initialize BLE with the device name
  Dabble.begin("UGV_ESP32");

  Serial.println("Dabble BLE initialized");
}

void loop() {

  // Receive and process commands from Dabble
  Dabble.processInput();

}
