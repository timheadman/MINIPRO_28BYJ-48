#include <Arduino.h>
#include <CheapStepper.h>

#include "DHTesp.h"

#define PIN_MIN_POSITION_SWITCH 2  // INT0
#define PIN_MAX_POSITION_SWITCH 3  // INT1
#define PIN_STEPPER_1 4
#define PIN_STEPPER_2 5
#define PIN_STEPPER_3 6
#define PIN_STEPPER_4 7
#define PIN_MOTOR_POWER 8
#define PIN_DHT22 9
#define PIN_LED 13

#define MAX_TURN_LIMIT 5
#define MAX_RPM 12

CheapStepper stepper(PIN_STEPPER_1, PIN_STEPPER_2, PIN_STEPPER_3,
                     PIN_STEPPER_4);
DHTesp dht;
uint32_t timerMillis = 0;
volatile int32_t currentTurn = 0;

void pushActuator();
void pullActuator();
void powerOnStepper();
void powerOffStepper();
void gotoZero();
void interruptMinLimitSwitch();
void interruptMaxLimitSwitch();

void setup() {
  Serial.begin(115200);
  dht.setup(PIN_DHT22, DHTesp::DHT22);
  pinMode(PIN_MOTOR_POWER, OUTPUT);
  pinMode(PIN_MIN_POSITION_SWITCH, INPUT_PULLUP);
  pinMode(PIN_MAX_POSITION_SWITCH, INPUT_PULLUP);
  attachInterrupt(0, interruptMinLimitSwitch, FALLING);
  attachInterrupt(1, interruptMaxLimitSwitch, FALLING);
  stepper.setRpm(MAX_RPM);
  gotoZero();
  Serial.print("[SETUP] Complete.\n");
}

void loop() {
  static float humidity = 0.0;
  static float temperature = 0.0;

  if (millis() > timerMillis + 1000) {
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    timerMillis = millis();
    Serial.print("T: " + String(temperature) + " H: " + String(humidity) +
                 "\n");
    pushActuator();
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
}

/* Установка актуатора в 0 положение.
 * Функция независима от других функций и работает напрямую с пинами
 * контроллера. */
void gotoZero() {
  digitalWrite(PIN_MOTOR_POWER, HIGH);
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

void pushActuator() {
  uint16_t maxSwitchValue = digitalRead(PIN_MAX_POSITION_SWITCH);
  if (maxSwitchValue && currentTurn < MAX_TURN_LIMIT) {
    powerOnStepper();
    stepper.moveDegreesCW(360);
    ++currentTurn;
    Serial.print("Turn: " + String(currentTurn) + "\n");
  }
  powerOffStepper();
}

void pullActuator() {
  if (digitalRead(PIN_MIN_POSITION_SWITCH)) {
    powerOnStepper();
    stepper.moveDegreesCCW(360);
    --currentTurn;
    Serial.print("Turn: " + String(currentTurn) + "\n");
  }
  powerOffStepper();
}
