#include <Arduino.h>
#include <CheapStepper.h>
#define PIN_MOTOR_POWER 6

CheapStepper stepper(2, 3, 4, 5);
bool moveClockwise = true;
unsigned long moveStartTime = 0;

void setup() {
  Serial.begin(115200);
  stepper.setRpm(24);
  pinMode(PIN_MOTOR_POWER, OUTPUT);
}

void loop() {
  static uint32_t counter = 0;
  stepper.run();
  digitalWrite(PIN_MOTOR_POWER, counter % 2);

  if (stepper.getStepsLeft() == 0) {
    Serial.println("Turn " + String(++counter) + " - " +
                   String(millis() - moveStartTime) + " ms.");
    stepper.newMoveDegreesCW(360);
    moveStartTime = millis();
  }
}
