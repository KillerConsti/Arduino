#include <PowerFunctions.h>

PowerFunctions pf(26, 0);

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  Serial.println("yay1");
}

void loop() {
  // put your main code here, to run repeatedly:
  step(RED, PWM_FWD7, 500);
  delay(1000);
  Serial.println("yay");
}

void step(uint8_t output, uint8_t pwm,  uint16_t time) {
  pf.single_pwm(output, pwm);
  delay(time);
  pf.single_pwm(output, PWM_BRK);
  delay(30);
  pf.single_pwm(output, PWM_FLT);
}
