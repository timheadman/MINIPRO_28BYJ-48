#include <Arduino.h>
#include <CheapStepper.h>

#define PIN_ZERO_POSITION_SWITCH 2  // INT0
#define PIN_STEPPER_1 4
#define PIN_STEPPER_2 5
#define PIN_STEPPER_3 6
#define PIN_STEPPER_4 7
#define PIN_MOTOR_POWER 8
#define PIN_LED 13

CheapStepper stepper(PIN_STEPPER_1, PIN_STEPPER_2, PIN_STEPPER_3,
                     PIN_STEPPER_4);
int32_t currentTurn = -1;

void turnToPush(uint16_t turns);
void turnToPull(uint16_t turns);
void powerOnStepper();
void powerOffStepper();
void gotoZero();
void interruptMinLimitSwitch();

void setup() {
  Serial.begin(115200);
  stepper.setRpm(24);
  pinMode(PIN_MOTOR_POWER, OUTPUT);
  pinMode(PIN_ZERO_POSITION_SWITCH, INPUT_PULLUP);
  attachInterrupt(0, interruptMinLimitSwitch, FALLING);
  gotoZero();
}

void loop() {
  stepper.run();

  if (stepper.getStepsLeft() == 0) {
    Serial.println("[" + String(currentTurn) + "] " +
                   String(digitalRead(PIN_ZERO_POSITION_SWITCH)));

    turnToPush(1);
    delay(3000);
  }
}

void interruptMinLimitSwitch() {
  powerOffStepper();
  stepper.stop();
  currentTurn = 0;
}

void gotoZero() {
  powerOnStepper();
  turnToPull((UINT16_MAX / 2 / 360) - 1);
}

void powerOnStepper() { digitalWrite(PIN_MOTOR_POWER, HIGH); }
void powerOffStepper() { digitalWrite(PIN_MOTOR_POWER, LOW); }

void turnToPush(uint16_t turns) {
  ++currentTurn;
  powerOnStepper();
  stepper.newMoveDegreesCW(360 * turns);
}

void turnToPull(uint16_t turns) {
  if (digitalRead(PIN_ZERO_POSITION_SWITCH)) {
    --currentTurn;
    powerOnStepper();
    stepper.newMoveDegreesCCW(360 * turns);
  } else {
    Serial.println("[ERROR] Attempt to open when zero switch is TRUE.");
  }
};
