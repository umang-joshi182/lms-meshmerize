// test_end_box.cpp - checks the end-box helper on hard-coded samples.
#include <iostream>
#include "../lms_esp32/lms_core.h"
using namespace std;

int failures = 0;
void check(const char *name, bool ok) {
  cout << (ok ? "[PASS] " : "[FAIL] ") << name << endl;
  if (!ok) failures++;
}

int main() {
  int all[5]  = {900, 950, 1000, 920, 880};
  int one[5]  = {900, 950, 1000, 920, 400};
  int none[5] = {0, 0, 0, 0, 0};
  int edge[5] = {700, 700, 700, 700, 700};

  check("all five above threshold -> true", allSensorsOnLine(all, 700));
  check("one sensor below threshold -> false", !allSensorsOnLine(one, 700));
  check("no line -> false", !allSensorsOnLine(none, 700));
  check("exactly at threshold counts as on the line", allSensorsOnLine(edge, 700));

  cout << (failures == 0 ? "All tests passed" : "Some tests FAILED") << endl;
  return failures;
}
