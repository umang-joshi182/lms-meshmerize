// lms_core.h - hardware-independent line-follower logic (PD/PID).
// Used by the ESP32 sketch and by the desktop test (src/test/test_pid.cpp).
#pragma once
#include <math.h>

#define NUM_SENSORS 5

// Sensor positions, left to right.
const int WEIGHTS[NUM_SENSORS] = {-2, -1, 0, 1, 2};

struct PidParams {
  float kp, ki, kd;     // PID gains
  float integralLimit;  // anti-windup clamp on the integral term
  int baseSpeed;        // forward speed (PWM units, 0-255)
  int maxSpeed;         // motor command limit
  int recoverSpeed;     // pivot speed while searching for a lost line
  int lostLimit;        // loops to search before giving up and stopping
  int minTotal;         // below this total reading, treat the line as lost
};

struct PidState {
  float prevError = 0;
  float integral = 0;
  float lastPosition = 0;  // last seen line side, used for recovery
  int lostCount = 0;
};

struct MotorCmd {
  int left;
  int right;
  bool stopped;
};

// Weighted centroid. Returns position in [-2, +2]; lineFound = false if the
// total reading is below minTotal (all-zero case handled before dividing).
inline float computeLinePosition(const int s[NUM_SENSORS], bool &lineFound, int minTotal) {
  long weightedSum = 0, total = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    weightedSum += (long)s[i] * WEIGHTS[i];
    total += s[i];
  }
  if (total < minTotal || total == 0) {
    lineFound = false;
    return 0.0f;
  }
  lineFound = true;
  return (float)weightedSum / total;
}

// Clamp to [-maxMag, +maxMag], sign preserved.
inline int clampMotor(int cmd, int maxMag) {
  if (cmd > maxMag) return maxMag;
  if (cmd < -maxMag) return -maxMag;
  return cmd;
}

// One control step: sensors in, motor commands out.
inline MotorCmd lineFollowStep(const int s[NUM_SENSORS], PidState &st,
                               const PidParams &p, float dt, float *errorOut = nullptr) {
  bool found;
  float pos = computeLinePosition(s, found, p.minTotal);
  MotorCmd cmd = {0, 0, false};

  // Lost line: pivot toward the side where the line was last seen,
  // and stop completely if it is not found within lostLimit loops.
  if (!found) {
    st.lostCount++;
    st.integral = 0;  // do not carry old error into recovery
    if (errorOut) *errorOut = 0;
    if (st.lostCount > p.lostLimit) {
      cmd.stopped = true;  // safe stop
    } else {
      int dir = (st.lastPosition >= 0) ? 1 : -1;
      cmd.left = clampMotor(dir * p.recoverSpeed, p.maxSpeed);
      cmd.right = clampMotor(-dir * p.recoverSpeed, p.maxSpeed);
    }
    return cmd;
  }

  float error = pos;  // target position is 0 (line under the centre sensor)
  if (st.lostCount > 0) st.prevError = error;  // avoid a derivative spike on reacquire
  st.lostCount = 0;
  st.lastPosition = pos;

  st.integral += error * dt;
  if (st.integral > p.integralLimit) st.integral = p.integralLimit;
  if (st.integral < -p.integralLimit) st.integral = -p.integralLimit;

  float derivative = (error - st.prevError) / dt;
  st.prevError = error;

  float correction = p.kp * error + p.ki * st.integral + p.kd * derivative;

  // Positive error (line on the right): left motor faster, right motor slower.
  cmd.left = clampMotor((int)lroundf(p.baseSpeed + correction), p.maxSpeed);
  cmd.right = clampMotor((int)lroundf(p.baseSpeed - correction), p.maxSpeed);
  if (errorOut) *errorOut = error;
  return cmd;
}// ---- Competition addition: end-box detection helper ----
// True when every normalised sensor reading (0..1000, line = high) is at or above 'threshold'.
// A perpendicular junction stripe also trips all five sensors, but only briefly, so the
// caller must require this to hold for a minimum time before declaring the end box.
inline bool allSensorsOnLine(const int s[NUM_SENSORS], int threshold) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (s[i] < threshold) return false;
  }
  return true;
}
