// lms_esp32.ino - PD/PID line follower for ESP32-WROOM-32 + L298N.
// All control logic lives in lms_core.h (tested on desktop by src/test/test_pid.cpp).
#include "lms_core.h"

// ---------------- Pins (Appendix A) ----------------
const int SENSOR_PINS[NUM_SENSORS] = {34, 35, 32, 33, 25};  // S1 (left) .. S5 (right)
const int ENA = 14, IN1 = 27, IN2 = 26;   // left motor
const int ENB = 13, IN3 = 12;             // right motor
// Appendix A lists IN4 = GPIO 11, but GPIO 6-11 are wired to the module's SPI flash
// on the WROOM-32 and cannot be used as I/O. Remapped to GPIO 4 (see README).
const int IN4 = 4;

// ---------------- Tuning ----------------
// kp, ki, kd, integralLimit, baseSpeed, maxSpeed, recoverSpeed, lostLimit, minTotal
PidParams params = {35.0f, 0.0f, 1.5f, 50.0f, 150, 255, 120, 50, 200};
const unsigned long LOOP_US = 10000;  // 10 ms -> 100 Hz (lostLimit 50 = ~0.5 s)

PidState state;
int rawMin[NUM_SENSORS], rawMax[NUM_SENSORS];
unsigned long lastMicros = 0;

// Min-max calibration: sweep the array over the line and the floor for 3 s.
void calibrate() {
  for (int i = 0; i < NUM_SENSORS; i++) { rawMin[i] = 4095; rawMax[i] = 0; }
  Serial.println("Calibrating: move the robot over line and floor...");
  unsigned long start = millis();
  while (millis() - start < 3000) {
    for (int i = 0; i < NUM_SENSORS; i++) {
      int v = analogRead(SENSOR_PINS[i]);
      if (v < rawMin[i]) rawMin[i] = v;
      if (v > rawMax[i]) rawMax[i] = v;
    }
    delay(2);
  }
  Serial.println("Calibration done.");
}

// Read and normalise to 0..1000 (higher = darker = line).
void readSensors(int out[NUM_SENSORS]) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    int raw = analogRead(SENSOR_PINS[i]);
    int range = rawMax[i] - rawMin[i];
    if (range < 100) { out[i] = 0; continue; }  // sensor saw no contrast: ignore it
    long v = (long)(raw - rawMin[i]) * 1000 / range;
    if (v < 0) v = 0;
    if (v > 1000) v = 1000;
    out[i] = (int)v;
  }
}

// Signed command: positive = forward, negative = reverse, magnitude = PWM (0-255).
void setMotor(int cmd, int inA, int inB, int en) {
  if (cmd > 0)      { digitalWrite(inA, HIGH); digitalWrite(inB, LOW); }
  else if (cmd < 0) { digitalWrite(inA, LOW);  digitalWrite(inB, HIGH); }
  else              { digitalWrite(inA, LOW);  digitalWrite(inB, LOW); }
  analogWrite(en, abs(cmd));
}

void setup() {
  Serial.begin(115200);
  int outs[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int i = 0; i < 6; i++) pinMode(outs[i], OUTPUT);
  for (int i = 0; i < NUM_SENSORS; i++) pinMode(SENSOR_PINS[i], INPUT);
  setMotor(0, IN1, IN2, ENA);
  setMotor(0, IN3, IN4, ENB);
  calibrate();
  delay(1000);  // time to place the robot on the line
  lastMicros = micros();
}

void loop() {
  unsigned long now = micros();
  if (now - lastMicros < LOOP_US) return;
  float dt = (now - lastMicros) / 1e6f;
  lastMicros = now;

  int s[NUM_SENSORS];
  readSensors(s);

  float err = 0;
  MotorCmd c = lineFollowStep(s, state, params, dt, &err);
  if (c.stopped) { c.left = 0; c.right = 0; }

  setMotor(c.left, IN1, IN2, ENA);
  setMotor(c.right, IN3, IN4, ENB);

  // Telemetry every loop: sensors, error, motor commands.
  Serial.print(s[0]); Serial.print(',');
  Serial.print(s[1]); Serial.print(',');
  Serial.print(s[2]); Serial.print(',');
  Serial.print(s[3]); Serial.print(',');
  Serial.print(s[4]); Serial.print(" | err=");
  Serial.print(err); Serial.print(" L=");
  Serial.print(c.left); Serial.print(" R=");
  Serial.println(c.right);
}