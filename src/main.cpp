#include <Arduino.h>
#include <TinyStepper_28BYJ_48.h>

const int LED_PIN = 13;
const int MOTOR_IN1_PIN = 2;
const int MOTOR_IN2_PIN = 3;
const int MOTOR_IN3_PIN = 4;
const int MOTOR_IN4_PIN = 5;
const int STOP_BUTTON_PIN = 9;

const int STEPS_PER_REVOLUTION = 2048;

TinyStepper_28BYJ_48 stepper;

boolean pushLinearActuator(uint16_t steps);
boolean pullLinearActuator(uint16_t steps);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, LOW);

  stepper.connectToPins(MOTOR_IN1_PIN, MOTOR_IN2_PIN, MOTOR_IN3_PIN,
                        MOTOR_IN4_PIN);
}

void loop() {
  Serial.println(pushLinearActuator(1));
  delay(3000);
}

/* Push the linear actuator for N steps. */
boolean pushLinearActuator(uint16_t steps) {
  stepper.setCurrentPositionInSteps(0);
  stepper.setupMoveInSteps(2048 * steps);
  stepper.setSpeedInStepsPerSecond(500);
  stepper.setAccelerationInStepsPerSecondPerSecond(65536);

  while (!stepper.motionComplete()) {
    if ((digitalRead(STOP_BUTTON_PIN) == HIGH)) {
      stepper.processMovement();
    } else {
      return false;
    }
  }
  return true;
}

/* Pull the linear actuator for N steps. */
boolean pullLinearActuator(uint16_t steps) {
  return pushLinearActuator(-(steps));
}
