#include <Arduino.h>
#include <TurtleReceiver.h>
#include <ESP32Servo.h>

// ---- Drive motors (L298N #1) ----
const int L_IN1 = 27, L_IN2 = 26, L_EN = 14;
const int R_IN1 = 25, R_IN2 = 33, R_EN = 32;

// ---- Conveyor motor (L298N #2 channel A) ----
const int C_IN1 = 18, C_IN2 = 19, C_EN = 5;

// ---- Servo ----
const int SERVO_PIN = 13;
Servo myServo;

const int SERVO_OFF_ANGLE = 90;   // center/resting position
const int SERVO_ON_ANGLE  = 0;    // 90 degrees to the left

NetController controller;

bool conveyorOn  = false;
bool servoOn     = false;
bool prevAButton = false;
bool prevBButton = false;

void setMotor(int in1, int in2, int en, int speed) {
  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(en, HIGH);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(en, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(en, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  controller.controllerSetup();

  int pins[] = {L_IN1, L_IN2, L_EN, R_IN1, R_IN2, R_EN, C_IN1, C_IN2, C_EN};
  for (int p : pins) pinMode(p, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_OFF_ANGLE);
}

void loop() {
  // ---- Tank steering ----
  float fwd  = controller.getJoy1Y();
  float turn = controller.getJoy2X();

  float leftF  = fwd + turn;
  float rightF = fwd - turn;

  int leftCmd  = (fabs(leftF)  > 0.3f) ? (leftF  > 0 ? 1 : -1) : 0;
  int rightCmd = (fabs(rightF) > 0.3f) ? (rightF > 0 ? 1 : -1) : 0;

  setMotor(L_IN1, L_IN2, L_EN, leftCmd);
  setMotor(R_IN1, R_IN2, R_EN, rightCmd);

  // ---- A button toggles conveyor ----
  bool aNow = controller.getA();
  if (aNow && !prevAButton) {
    conveyorOn = !conveyorOn;
  }
  prevAButton = aNow;

  if (conveyorOn) {
    digitalWrite(C_IN1, HIGH);
    digitalWrite(C_IN2, LOW);
    digitalWrite(C_EN,  HIGH);
  } else {
    digitalWrite(C_IN1, LOW);
    digitalWrite(C_IN2, LOW);
    digitalWrite(C_EN,  LOW);
  }

  // ---- B button toggles servo (90 degrees to the left) ----
  bool bNow = controller.getB();
  if (bNow && !prevBButton) {
    servoOn = !servoOn;
    myServo.write(servoOn ? SERVO_ON_ANGLE : SERVO_OFF_ANGLE);
  }
  prevBButton = bNow;

  delay(20);
}