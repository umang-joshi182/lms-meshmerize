// test_pid.cpp - runs the control logic against hard-coded sensor samples.
#include <iostream>
#include "../lms_esp32/lms_core.h"
using namespace std;

int main() {
  // kp, ki, kd, integralLimit, base, max, recover, lostLimit, minTotal
  PidParams p = {35.0f, 0.0f, 1.5f, 50.0f, 150, 255, 120, 3, 200};
  PidState st;
  const float dt = 0.01f;  // 10 ms loop

  int samples[][NUM_SENSORS] = {
    {0, 0, 3000, 0, 0},       // centred
    {0, 0, 3000, 1000, 0},    // slightly right
    {0, 0, 1000, 3000, 0},    // more right
    {0, 0, 3000, 0, 0},       // back to centre
    {1000, 3000, 0, 0, 0},    // line on the left
    {0, 0, 0, 0, 0},          // lost 1 -> recover
    {0, 0, 0, 0, 0},          // lost 2 -> recover
    {0, 0, 0, 0, 0},          // lost 3 -> recover
    {0, 0, 0, 0, 0},          // lost 4 -> safe stop
    {0, 0, 3000, 0, 0}        // line found again
  };

  int n = sizeof(samples) / sizeof(samples[0]);
  for (int i = 0; i < n; i++) {
    float err;
    MotorCmd c = lineFollowStep(samples[i], st, p, dt, &err);
    cout << "step " << i << ": error=" << err
         << "  left=" << c.left << "  right=" << c.right
         << (c.stopped ? "  [STOPPED]" : "") << endl;
  }
  return 0;
}