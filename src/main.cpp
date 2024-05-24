#include <Arduino.h>
#include <CheapStepper.h>

#define PIN_MIN_POSITION_SWITCH 2  // INT0
#define PIN_MAX_POSITION_SWITCH 3  // INT1
#define PIN_STEPPER_1 4
#define PIN_STEPPER_2 5
#define PIN_STEPPER_3 6
#define PIN_STEPPER_4 7
#define PIN_MOTOR_POWER 8
#define PIN_LED 13
#define MAX_TURN_LIMIT 10

#define MAX_CW_RPM 24
#define MAX_CCW_RPM 15

CheapStepper stepper(PIN_STEPPER_1, PIN_STEPPER_2, PIN_STEPPER_3,
                     PIN_STEPPER_4);
int32_t currentTurn = 0;
int32_t maxTurn = 0;

void turnToPush(uint16_t turns);
void turnToPull(uint16_t turns);
boolean powerOnStepper();
void powerOffStepper();
void gotoZero();
void interruptMinLimitSwitch();
void interruptMaxLimitSwitch();

void setup() {
  Serial.begin(115200);
  pinMode(PIN_MOTOR_POWER, OUTPUT);
  pinMode(PIN_MIN_POSITION_SWITCH, INPUT_PULLUP);
  pinMode(PIN_MAX_POSITION_SWITCH, INPUT_PULLUP);
  attachInterrupt(0, interruptMinLimitSwitch, FALLING);
  attachInterrupt(1, interruptMaxLimitSwitch, FALLING);
  gotoZero();
}

void loop() {
  // stepper.run();

  // if (stepper.getStepsLeft() == 0) {
  //   Serial.println("[" + String(currentTurn) + "] " +
  //                  String(digitalRead(PIN_MIN_POSITION_SWITCH)));

  //   turnToPush(1);
  //   delay(3000);
  // }
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

void gotoZero() {
  if (powerOnStepper()) {
    while (true) {
      stepper.setRpm(MAX_CCW_RPM);
      stepper.moveDegreesCCW(360);
    }
    powerOffStepper();
  }
}

boolean powerOnStepper() {
  int16_t minSwitchStatus = digitalRead(PIN_MIN_POSITION_SWITCH);
  int16_t maxSwitchStatus = digitalRead(PIN_MAX_POSITION_SWITCH);

  if (minSwitchStatus || maxSwitchStatus || currentTurn <= MAX_TURN_LIMIT) {
    digitalWrite(PIN_MOTOR_POWER, HIGH);
    return true;
  } else {
    Serial.println("[ERROR] Bad attempt to power on stepper. Current turn: " +
                   String(currentTurn) +
                   ", PIN_MIN_POSITION_SWITCH = " + String(minSwitchStatus) +
                   ", PIN_MAX_POSITION_SWITCH = " + String(maxSwitchStatus) +
                   ", MAX_TURN_LIMIT = " + String(MAX_TURN_LIMIT) + ".");
    return false;
  }
}

void powerOffStepper() { digitalWrite(PIN_MOTOR_POWER, LOW); }

void turnToPush(uint16_t turns) {
  if (powerOnStepper()) {
    ++currentTurn;
    maxTurn < currentTurn ? maxTurn = currentTurn : false;
    stepper.newMoveDegreesCW(360 * turns);
  }
}

void turnToPull(uint16_t turns) {
  if (powerOnStepper()) {
    --currentTurn;
    stepper.newMoveDegreesCCW(360 * turns);
  }
};
