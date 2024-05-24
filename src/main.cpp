#include <Arduino.h>
#include <CheapStepper.h>

#define PIN_ZERO_POSITION_SWITCH 2  // INT0
#define PIN_STEPPER_1 4
#define PIN_STEPPER_2 5
#define PIN_STEPPER_3 6
#define PIN_STEPPER_4 7
#define PIN_MOTOR_POWER 8
#define PIN_LED 13

void processZeroPositionInterrupt();
void makeTurnCW(uint16_t turns);
void powerOnStepper();
void powerOffStepper();

CheapStepper stepper(PIN_STEPPER_1, PIN_STEPPER_2, PIN_STEPPER_3,
                     PIN_STEPPER_4);
bool moveClockwise = true;
uint32_t moveStartTime = 0;
uint16_t intCounter = 0;

void setup() {
  Serial.begin(115200);
  stepper.setRpm(24);
  pinMode(PIN_MOTOR_POWER, OUTPUT);
  pinMode(PIN_ZERO_POSITION_SWITCH, INPUT_PULLUP);
  attachInterrupt(0, processZeroPositionInterrupt, FALLING);
}

void loop() {
  static uint32_t counter = 0;
  stepper.run();
  counter % 2 ? powerOnStepper() : powerOffStepper();

  if (stepper.getStepsLeft() == 0) {
    Serial.println("Turn " + String(++counter) + " - " +
                   String(millis() - moveStartTime) + " ms. [" +
                   String(intCounter) + "]");
    makeTurnCW(1);
    moveStartTime = millis();
  }
}

void powerOnStepper() { digitalWrite(PIN_MOTOR_POWER, HIGH); }

void powerOffStepper() { digitalWrite(PIN_MOTOR_POWER, LOW); }

void makeTurnCW(uint16_t turns) { stepper.newMoveDegreesCW(360 * turns); }

void processZeroPositionInterrupt() {
  powerOffStepper();
  stepper.stop();
  intCounter++;
}