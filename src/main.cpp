#include <Arduino.h>
#include <TurtleReceiver.h>

// ---- Drive motors (L298N #1) ----
const int L_IN1 = 27, L_IN2 = 26, L_EN = 14;   // Left wheel
const int R_IN1 = 25, R_IN2 = 33, R_EN = 32;   // Right wheel

// ---- Conveyor motor (L298N #2 channel A) ----
const int C_IN1 = 18, C_IN2 = 19, C_EN = 5;

const float DEADZONE = 0.15f;   // ignore tiny joystick noise
const int   MAX_PWM  = 255;

NetController controller;

bool conveyorOn  = false;
bool prevAButton = false;       // edge-detect so each press toggles once

void driveMotor(int in1, int in2, int en, float speed) {
  // speed in [-1, 1]
  if (fabs(speed) < DEADZONE) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(en, 0);
    return;
  }
  bool forward = speed > 0;
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW  : HIGH);
  analogWrite(en, (int)(fabs(speed) * MAX_PWM));
}

void setup() {
  Serial.begin(115200);
  controller.controllerSetup();
  printMacAddress();   // remove this line once you've recorded the MAC

  int pins[] = {L_IN1, L_IN2, L_EN, R_IN1, R_IN2, R_EN, C_IN1, C_IN2, C_EN};
  for (int p : pins) pinMode(p, OUTPUT);
}

void loop() {
  // ---- Arcade drive: left stick Y = forward/back, right stick X = turn ----
  float fwd  = controller.getJoy1Y();
  float turn = controller.getJoy2X();

  float left  = constrain(fwd + turn, -1.0f, 1.0f);
  float right = constrain(fwd - turn, -1.0f, 1.0f);

  driveMotor(L_IN1, L_IN2, L_EN, left);
  driveMotor(R_IN1, R_IN2, R_EN, right);

  // ---- A button toggles the conveyor (edge-detect on press) ----
  bool aNow = controller.getA();
  if (aNow && !prevAButton) {
    conveyorOn = !conveyorOn;
  }
  prevAButton = aNow;

  driveMotor(C_IN1, C_IN2, C_EN, conveyorOn ? 1.0f : 0.0f);

  delay(20);
}
