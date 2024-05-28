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

#define MAX_TURN_LIMIT 5
#define MAX_CW_RPM 24
#define MAX_CCW_RPM 12

CheapStepper stepper(PIN_STEPPER_1, PIN_STEPPER_2, PIN_STEPPER_3,
                     PIN_STEPPER_4);
volatile int32_t currentTurn = 0;
volatile int32_t maxTurn = 0;

void turnToPush(uint16_t turns);
void turnToPull(uint16_t turns);
void powerOnStepper();
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
  Serial.print("[SETUP] Complete.\n");
}

void loop() {
  stepper.run();
  if (stepper.getStepsLeft() == 0) {
    Serial.print("[LOOP] Current turn: " + String(currentTurn) + "\n");
    turnToPush(1);
    delay(3000);
  }
}

void interruptMinLimitSwitch() {
  // powerOffStepper();
  stepper.stop();
  currentTurn = 0;
}

void interruptMaxLimitSwitch() {
  powerOffStepper();
  stepper.stop();
  maxTurn = currentTurn;
}

/* Установка актуатора в 0 положение.
 * Функция независима от других функций и работает напрямую с пинами
 * контроллера. */
void gotoZero() {
  digitalWrite(PIN_MOTOR_POWER, HIGH);
  stepper.setRpm(MAX_CCW_RPM);
  currentTurn = -1;
  Serial.print("[CALIBRATION] Start ");
  while (currentTurn) {
    stepper.newMoveDegreesCCW(360);
    while (stepper.getStepsLeft()) stepper.run();
    if (currentTurn != 0) {
      Serial.print(String(currentTurn) + " ");
      --currentTurn;
    } else {
      Serial.print("\n[CALIBRATION] Complete.\n");
    }
  }
  powerOffStepper();
}

void powerOnStepper() {
  if (!digitalRead(PIN_MIN_POSITION_SWITCH) &&
      !digitalRead(PIN_MAX_POSITION_SWITCH)) {
    Serial.print("[ERROR] Both limit switches are closed.\n");
  } else {
    digitalWrite(PIN_MOTOR_POWER, HIGH);
  }
}

void powerOffStepper() { digitalWrite(PIN_MOTOR_POWER, LOW); }

void turnToPush(uint16_t turns) {
  if (digitalRead(PIN_MAX_POSITION_SWITCH) || currentTurn < MAX_TURN_LIMIT) {
    powerOnStepper();
    stepper.setRpm(MAX_CW_RPM);
    ++currentTurn;
    maxTurn < currentTurn ? maxTurn = currentTurn : false;
    stepper.newMoveDegreesCW(360 * turns);
  }
}

void turnToPull(uint16_t turns) {
  if (digitalRead(PIN_MIN_POSITION_SWITCH)) {
    powerOnStepper();
    stepper.setRpm(MAX_CCW_RPM);
    --currentTurn;
    stepper.newMoveDegreesCCW(360 * turns);
  }
};
