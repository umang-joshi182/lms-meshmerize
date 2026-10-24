#include <iostream>
#include <cmath>
using namespace std;

const int NUM_SENSORS = 5;
// Sensor positions, left to right.
const int WEIGHTS[NUM_SENSORS] = {-2, -1, 0, 1, 2};

// Task 1.1: weighted centroid of 5 calibrated, non-negative readings.
// Returns the line position in [-2, +2] (negative = left, positive = right).
// If the total reading is below minTotal (all-zero by default), no line is visible:
// sets lineFound = false and returns 0.
float computeLinePosition(const int s[NUM_SENSORS], bool &lineFound, int minTotal = 1) {
    long weightedSum = 0;  // sum of (reading * position)
    long total = 0;        // sum of readings
    for (int i = 0; i < NUM_SENSORS; i++) {
        long reading = (s[i] > 0) ? s[i] : 0;  // a negative reading is invalid: treat as 0
        weightedSum += reading * WEIGHTS[i];
        total += reading;
    }

    if (minTotal < 1) minTotal = 1;  // guarantees total >= 1 below, so no division by zero
    if (total < minTotal) {
        lineFound = false;
        return 0.0f;
    }

    lineFound = true;
    return (float)weightedSum / (float)total;  // float division, not integer division
}

// Task 1.2: limit a signed motor command to [-maxMag, +maxMag]; sign is preserved.
int clampMotor(int cmd, int maxMag) {
    if (maxMag < 0) maxMag = -maxMag;  // only the size of the limit matters
    if (cmd > maxMag)  return maxMag;
    if (cmd < -maxMag) return -maxMag;
    return cmd;
}

// ---------------- examples and self-tests ----------------

void show(const char *name, const int s[NUM_SENSORS]) {
    bool found;
    float pos = computeLinePosition(s, found);
    cout << name << " -> position: " << pos
         << ", lineFound: " << (found ? "true" : "false") << endl;
}

int failures = 0;
void check(const char *name, bool ok) {
    cout << (ok ? "[PASS] " : "[FAIL] ") << name << endl;
    if (!ok) failures++;
}
bool approx(float a, float b) { return fabs(a - b) < 0.0001f; }

void runTests() {
    bool f;

    int centre[5]    = {0, 0, 4000, 0, 0};
    int rightEdge[5] = {0, 0, 0, 0, 4000};
    int leftEdge[5]  = {4000, 0, 0, 0, 0};
    int between[5]   = {0, 0, 2000, 2000, 0};
    int mixed[5]     = {1000, 3000, 0, 0, 0};
    int mirrored[5]  = {0, 0, 0, 3000, 1000};   // mirror image of mixed
    int scaled[5]    = {3000, 9000, 0, 0, 0};   // mixed x 3
    int zeros[5]     = {0, 0, 0, 0, 0};
    int badInput[5]  = {-500, 0, 4000, 0, 0};   // negative reading is ignored
    int weak[5]      = {10, 0, 0, 0, 0};        // very weak signal

    check("centre -> 0",          approx(computeLinePosition(centre, f), 0.0f) && f);
    check("right edge -> +2",     approx(computeLinePosition(rightEdge, f), 2.0f) && f);
    check("left edge -> -2",      approx(computeLinePosition(leftEdge, f), -2.0f) && f);
    check("between S3,S4 -> 0.5", approx(computeLinePosition(between, f), 0.5f) && f);
    check("mixed -> -1.25",       approx(computeLinePosition(mixed, f), -1.25f) && f);

    float pm = computeLinePosition(mixed, f);
    float pr = computeLinePosition(mirrored, f);
    check("mirror image gives the opposite position", approx(pm, -pr));

    float ps = computeLinePosition(scaled, f);
    check("scaling all readings by 3 does not change the position", approx(pm, ps));

    float pz = computeLinePosition(zeros, f);
    check("all zero -> lineFound false, position 0", !f && approx(pz, 0.0f));

    float pb = computeLinePosition(badInput, f);
    check("negative reading is ignored", approx(pb, 0.0f) && f);

    computeLinePosition(weak, f, 200);
    check("weak signal below minTotal -> lineFound false", !f);
    float pw = computeLinePosition(weak, f);
    check("same weak signal with default minTotal -> found, -2", f && approx(pw, -2.0f));

    check("clamp(300, 255) = 255",   clampMotor(300, 255) == 255);
    check("clamp(-300, 255) = -255", clampMotor(-300, 255) == -255);
    check("clamp(100, 255) = 100",   clampMotor(100, 255) == 100);
    check("clamp(-100, 255) = -100", clampMotor(-100, 255) == -100);
    check("clamp(255, 255) = 255 (boundary)",   clampMotor(255, 255) == 255);
    check("clamp(-255, 255) = -255 (boundary)", clampMotor(-255, 255) == -255);
    check("clamp(0, 255) = 0",       clampMotor(0, 255) == 0);
    check("clamp(50, 0) = 0 (limit zero)", clampMotor(50, 0) == 0);
    check("clamp(300, -255) = 255 (negative limit treated as its size)", clampMotor(300, -255) == 255);
}

int main() {
    cout << "--- computeLinePosition examples ---" << endl;
    int centre[5]    = {0, 0, 4000, 0, 0};
    int rightEdge[5] = {0, 0, 0, 0, 4000};
    int leftEdge[5]  = {4000, 0, 0, 0, 0};
    int between[5]   = {0, 0, 2000, 2000, 0};
    int mixed[5]     = {1000, 3000, 0, 0, 0};
    int none[5]      = {0, 0, 0, 0, 0};
    show("centre   ", centre);
    show("rightEdge", rightEdge);
    show("leftEdge ", leftEdge);
    show("between  ", between);
    show("mixed    ", mixed);
    show("allZero  ", none);

    cout << "--- clampMotor examples (max = 255) ---" << endl;
    cout << "clamp(300)  = " << clampMotor(300, 255)  << endl;
    cout << "clamp(-300) = " << clampMotor(-300, 255) << endl;
    cout << "clamp(100)  = " << clampMotor(100, 255)  << endl;
    cout << "clamp(-100) = " << clampMotor(-100, 255) << endl;

    cout << "--- self-tests ---" << endl;
    runTests();
    cout << (failures == 0 ? "All tests passed" : "Some tests FAILED") << endl;
    return failures;  // 0 means success
}