#include <Arduino.h>
#include <CheapStepper.h>

CheapStepper stepper(2, 3, 4, 5);

void setup() {
  Serial.begin(115200);
  stepper.setRpm(10);
  Serial.println();
  Serial.print(stepper.getRpm());
  Serial.print(" rpm = delay of ");
  Serial.print(stepper.getDelay());
  Serial.println(" microseconds between steps");

  // stepper.setTotalSteps(4076);
  /* you can uncomment the above line if you think your motor
   * is geared 63.68395:1 (measured) rather than 64:1 (advertised)
   * which would make the total steps 4076 (rather than default 4096)
   * for more info see: http://forum.arduino.cc/index.php?topic=71964.15
   */
}

void loop() {
  static uint16_t counter = 0;
  counter++;
  stepper.moveDegreesCW(360 * counter);
  Serial.println("[DEGREES] Step position: " + String(stepper.getStep()) + " / 4096");
  delay(1000);
}

