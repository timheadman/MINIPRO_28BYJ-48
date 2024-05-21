#include <Arduino.h>
#include <CheapStepper.h>

CheapStepper stepper(2, 3, 4, 5);
bool moveClockwise = true;
unsigned long moveStartTime = 0;

void setup() {
  Serial.begin(115200);
  stepper.setRpm(24);
}

void loop() {
  stepper.run();

  if (stepper.getStepsLeft() == 0) {
    Serial.println("Turn " + String(millis() - moveStartTime) + " ms.");
    stepper.newMoveDegreesCW(360);
    moveStartTime = millis();
  }
}
