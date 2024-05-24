#include <Arduino.h>
#include <CheapStepper.h>

#define PIN_MIN_POSITION_SWITCH 2  // INT0
#define PIN_MAX_POSITION_SWITCH 2  // INT0
#define PIN_STEPPER_1 4
#define PIN_STEPPER_2 5
#define PIN_STEPPER_3 6
#define PIN_STEPPER_4 7
#define PIN_MOTOR_POWER 8
#define PIN_LED 13
#define MAX_TURN_LIMIT 10

CheapStepper stepper(PIN_STEPPER_1, PIN_STEPPER_2, PIN_STEPPER_3,
                     PIN_STEPPER_4);
int32_t currentTurn = -1;
int32_t maxTurn = 0;

void turnToPush(uint16_t turns);
void turnToPull(uint16_t turns);
void powerOnStepper();
void powerOffStepper();
void gotoZero();
void interruptMinLimitSwitch();
void interruptMaxLimitSwitch();

void setup() {
  Serial.begin(115200);
  stepper.setRpm(24);
  pinMode(PIN_MOTOR_POWER, OUTPUT);
  pinMode(PIN_MIN_POSITION_SWITCH, INPUT_PULLUP);
  pinMode(PIN_MAX_POSITION_SWITCH, INPUT_PULLUP);
  attachInterrupt(0, interruptMinLimitSwitch, FALLING);
  attachInterrupt(1, interruptMaxLimitSwitch, FALLING);
  gotoZero();
}

void loop() {
  stepper.run();

  if (stepper.getStepsLeft() == 0) {
    Serial.println("[" + String(currentTurn) + "] " +
                   String(digitalRead(PIN_MIN_POSITION_SWITCH)));

    turnToPush(1);
    delay(3000);
  }
}

void interruptMinLimitSwitch() {
  powerOffStepper();
  stepper.stop();
  currentTurn = 0;
}

void interruptMaxLimitSwitch() {
  powerOffStepper();
  stepper.stop();
  maxTurn = currentTurn;
}

void gotoZero() { turnToPull((UINT16_MAX / 2 / 360) - 1); }

void powerOnStepper() { digitalWrite(PIN_MOTOR_POWER, HIGH); }
void powerOffStepper() { digitalWrite(PIN_MOTOR_POWER, LOW); }

void turnToPush(uint16_t turns) {
  if (digitalRead(PIN_MAX_POSITION_SWITCH) || currentTurn != MAX_TURN_LIMIT) {
    ++currentTurn;
    maxTurn < currentTurn ? maxTurn = currentTurn : false;
    powerOnStepper();
    stepper.newMoveDegreesCW(360 * turns);
  } else {
    interruptMaxLimitSwitch();
    Serial.println(
        "[ERROR] Attempt to open when MAX switch is TRUE. Current turn: " +
        String(currentTurn) + ", MAX_TURN_LIMIT = " + String(MAX_TURN_LIMIT) +
        ".");
  }
}

void turnToPull(uint16_t turns) {
  if (digitalRead(PIN_MIN_POSITION_SWITCH)) {
    --currentTurn;
    powerOnStepper();
    stepper.newMoveDegreesCCW(360 * turns);
  } else {
    interruptMinLimitSwitch();
    Serial.println("[ERROR] Attempt to open when MIN switch is TRUE.");
  }
};
