#include <iostream>
using namespace std;

// Sensor positions, left to right.
const int WEIGHTS[5] = {-2, -1, 0, 1, 2};

// Task 1.1: weighted centroid of 5 calibrated, non-negative readings.
// Returns line position in [-2, +2] (negative = left, positive = right).
// If every reading is zero, no line is visible: sets lineFound = false and returns 0.
float computeLinePosition(const int s[5], bool &lineFound) {
    long weightedSum = 0;  // sum of (reading * position)
    long total = 0;        // sum of readings
    for (int i = 0; i < 5; i++) {
        weightedSum += (long)s[i] * WEIGHTS[i];
        total += s[i];
    }

    // Guard against division by zero (all sensors see no line).
    if (total == 0) {
        lineFound = false;
        return 0.0f;
    }

    lineFound = true;
    return (float)weightedSum / total;  // cast to float to avoid integer division
}

// Task 1.2: limit a signed motor command to [-maxMag, +maxMag]; sign is preserved.
int clampMotor(int cmd, int maxMag) {
    if (cmd > maxMag)  return maxMag;
    if (cmd < -maxMag) return -maxMag;
    return cmd;
}

void testPosition(const char *name, const int s[5]) {
    bool found;
    float pos = computeLinePosition(s, found);
    cout << name << " -> position: " << pos
         << ", lineFound: " << (found ? "true" : "false") << endl;
}

int main() {
    cout << "--- computeLinePosition ---" << endl;
    int centre[5]    = {0, 0, 4000, 0, 0};
    int rightEdge[5] = {0, 0, 0, 0, 4000};
    int leftEdge[5]  = {4000, 0, 0, 0, 0};
    int between[5]   = {0, 0, 2000, 2000, 0};
    int mixed[5]     = {1000, 3000, 0, 0, 0};
    int none[5]      = {0, 0, 0, 0, 0};
    testPosition("centre   ", centre);
    testPosition("rightEdge", rightEdge);
    testPosition("leftEdge ", leftEdge);
    testPosition("between  ", between);
    testPosition("mixed    ", mixed);
    testPosition("allZero  ", none);

    cout << "--- clampMotor (max = 255) ---" << endl;
    cout << "clamp(300)  = " << clampMotor(300, 255)  << endl;
    cout << "clamp(-300) = " << clampMotor(-300, 255) << endl;
    cout << "clamp(100)  = " << clampMotor(100, 255)  << endl;
    cout << "clamp(-100) = " << clampMotor(-100, 255) << endl;
    return 0;
}
