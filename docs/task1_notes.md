# Task 1: Basic Coding, explanation of logic

Code: `src/task1_basic.cpp`. Run: `g++ -std=c++11 src/task1_basic.cpp -o task1 && ./task1`

## 1. Line-position estimation (weighted centroid)

**How the sensors feed this.** Each IR sensor has an IR LED and a receiver. A dark surface (the line) reflects less light, which raises the sensor's analog output (0 to 3.3 V, read as 0 to 4095 by the ESP32). After calibration, each sensor gives a non-negative number that is larger over the line.

**Idea.** The five sensors sit at positions -2, -1, 0, +1, +2 (left to right). Each reading is a "weight" at its position, and the line position is the balance point of those weights:

`position = sum(reading_i x position_i) / sum(reading_i)`

A line directly under one sensor gives that sensor's position. A line between two sensors gives a value in between, so the estimate is continuous and finer than the 5 discrete sensors.

**All-zero case.** If every reading is zero, the denominator is zero and the formula is undefined. The function checks the total before dividing, sets `lineFound = false` and returns 0. The caller (Task 9) uses `lineFound` to start lost-line recovery, because returning 0 alone would be read as "line centred".

**Implementation details.**
- `weightedSum` and `total` are `long` so the products cannot overflow.
- The result is computed with float division. Without it, `long / long` is integer division and a value like 0.5 would become 0.

**Examples (from the program output).**

| Readings | Position | lineFound |
|----------|----------|-----------|
| {0, 0, 4000, 0, 0} | 0 | true |
| {0, 0, 0, 0, 4000} | 2 | true |
| {4000, 0, 0, 0, 0} | -2 | true |
| {0, 0, 2000, 2000, 0} | 0.5 | true |
| {1000, 3000, 0, 0, 0} | -1.25 | true |
| {0, 0, 0, 0, 0} | 0 | false |

Hand check for the fifth row: numerator = 1000 x (-2) + 3000 x (-1) = -5000, denominator = 4000, so -5000 / 4000 = -1.25.

## 2. Motor-command limiting

**Idea.** A signed command (positive = forward, negative = reverse) is limited to the range [-maxMag, +maxMag]:
- above +maxMag: return +maxMag
- below -maxMag: return -maxMag
- otherwise: return the command unchanged

Because the lower bound is -maxMag (not 0), a negative command stays negative, so the direction is preserved. The limit is needed because the PID output can exceed what the motor driver accepts (PWM 0 to 255). In Task 9 the left and right commands are each computed first and then clamped separately.

**Examples (maxMag = 255).**

| Command | Result |
|---------|--------|
| 300 | 255 |
| -300 | -255 |
| 100 | 100 |
| -100 | -100 |

## 3. Robustness choices and self-tests

- A negative reading is treated as 0, since calibrated readings must be non-negative; this stops one bad value from skewing the centroid.
- `minTotal` (default 1) is the smallest total reading accepted as a line. With the default only the all-zero case is rejected; Task 9 uses a larger value to ignore noise-level readings.
- A negative `maxMag` is treated as its absolute value.
- The program ends with self-tests (symmetry, scale invariance, boundary values, edge cases). Each prints `[PASS]` or `[FAIL]`, and the exit code is 0 only if all pass.

## 4. Assumptions

- Readings are calibrated, as stated in the task.
- A command exactly equal to the limit is returned unchanged.