// lms_esp32.ino - PD/PID line follower for ESP32-WROOM-32 + L298N.
// All control logic lives in lms_core.h (tested on desktop by src/test/test_pid.cpp).
// Race firmware note: Wi-Fi and Bluetooth are never started in this sketch
// (the Meshmerize rules require them to be off near the arena).
#include "lms_core.h"

// ---------------- Pins (Appendix A) ----------------
const int SENSOR_PINS[NUM_SENSORS] = {34, 35, 32, 33, 25};  // S1 (left) .. S5 (right)
const int ENA = 14, IN1 = 27, IN2 = 26;   // left motor
const int ENB = 13, IN3 = 12;             // right motor
// Appendix A lists IN4 = GPIO 11, but GPIO 6-11 are wired to the module's SPI flash
// on the WROOM-32 and cannot be used as I/O. Remapped to GPIO 4 (see README).
const int IN4 = 4;
const int END_LED = 23;  // red LED + series resistor, lights at the end zone

// ---------------- Line polarity ----------------
// true : the line gives the HIGHER analog reading (Appendix A: darker surface = higher voltage).
// false: the line gives the LOWER reading (for example a white line on a dark floor with a
//        module whose output drops over a reflective surface). Check on the bench first.
const bool LINE_IS_HIGH_RAW = true;

// ---------------- End-box detection (tuning values, not measured) ----------------
const int END_THRESHOLD = 700;         // normalised reading above this = sensor is on the line
const unsigned long END_BOX_MS = 250;  // all five on the line this long = end box

// ---------------- Control tuning ----------------
// kp, ki, kd, integralLimit, baseSpeed, maxSpeed, recoverSpeed, lostLimit, minTotal
PidParams params = {35.0f, 0.0f, 1.5f, 50.0f, 150, 255, 120, 50, 200};
const unsigned long LOOP_US = 10000;  // 10 ms -> 100 Hz (lostLimit 50 = ~0.5 s)

PidState state;
int rawMin[NUM_SENSORS], rawMax[NUM_SENSORS];
unsigned long lastMicros = 0;
bool finished = false;
unsigned long allOnLineSince = 0;  // 0 = not currently all on the line

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

// Read and normalise to 0..1000 with the LINE always the high value.
void readSensors(int out[NUM_SENSORS]) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    int raw = analogRead(SENSOR_PINS[i]);
    int range = rawMax[i] - rawMin[i];
    if (range < 100) { out[i] = 0; continue; }  // sensor saw no contrast: ignore it
    long v = (long)(raw - rawMin[i]) * 1000 / range;
    if (v < 0) v = 0;
    if (v > 1000) v = 1000;
    if (!LINE_IS_HIGH_RAW) v = 1000 - v;  // flip so that the line is high
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

void stopMotors() {
  setMotor(0, IN1, IN2, ENA);
  setMotor(0, IN3, IN4, ENB);
}

void setup() {
  Serial.begin(115200);
  int outs[] = {ENA, IN1, IN2, ENB, IN3, IN4, END_LED};
  for (int i = 0; i < 7; i++) pinMode(outs[i], OUTPUT);
  for (int i = 0; i < NUM_SENSORS; i++) pinMode(SENSOR_PINS[i], INPUT);
  digitalWrite(END_LED, LOW);
  stopMotors();
  calibrate();
  delay(1000);  // time to place the robot on the line
  lastMicros = micros();
}

void loop() {
  if (finished) {  // end box reached: stay stopped with the LED on
    stopMotors();
    digitalWrite(END_LED, HIGH);
    return;
  }

  unsigned long now = micros();
  if (now - lastMicros < LOOP_US) return;
  float dt = (now - lastMicros) / 1e6f;
  lastMicros = now;

  int s[NUM_SENSORS];
  readSensors(s);

  // End box: all five sensors on the line continuously for END_BOX_MS.
  if (allSensorsOnLine(s, END_THRESHOLD)) {
    if (allOnLineSince == 0) allOnLineSince = millis();
    else if (millis() - allOnLineSince >= END_BOX_MS) {
      finished = true;
      stopMotors();
      digitalWrite(END_LED, HIGH);
      Serial.println("END BOX reached");
      return;
    }
  } else {
    allOnLineSince = 0;
  }

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
