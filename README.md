
# LMS / Meshmerize: Fresher Selection Tasks

Submission for the nine selection tasks (ESP32, Arduino framework, C++; hardware as in Appendix A of the task sheet).

## Repository layout

| Path | Contents |
|------|----------|
| `src/task1_basic.cpp` | Task 1: weighted-centroid line position and motor clamp, with example inputs and outputs |
| `docs/task1_notes.md` | Task 1: explanation of the logic |
| `src/lms_esp32/lms_core.h` | Task 9: hardware-independent PD/PID control logic (centroid, PID, lost-line handling) |
| `src/lms_esp32/lms_esp32.ino` | Task 9: ESP32 sketch (pins, calibration, motor driver, telemetry) |
| `src/test/test_pid.cpp` | Task 9: desktop test that runs the control logic on hard-coded sensor samples |
| `docs/control_and_tuning.md` | Tasks 3 and 9: PID theory, configuration choice, tuning notes |
| `docs/datasheet_l298n.md` | Task 2: L298N datasheet analysis |
| `docs/wiring_and_troubleshooting.md` | Task 6: wiring diagram, ground topology, troubleshooting analysis |
| `docs/wireless_diagnostics.md` | Task 7: protocol choice (BLE) and architecture |
| `docs/maze_analysis.md` | Task 8: DFS vs wall rule, example maze, pseudocode |
| `BOM/power_budget.md` | Task 5: power budget, runtime, battery chemistry comparison |
| `BOM/bom.md` | Task 4: bill of materials (in progress, see Status) |

## Status

| Task | Status |
|------|--------|
| 1 Basic coding | Done, tested on desktop |
| 2 Datasheet literacy | Written |
| 3 PID control | Written (combined with Task 9 notes) |
| 4 BOM | Partial: rows 1 to 7 (controller, driver, sensors, motors, battery, charger, buck converter) chosen and priced; chassis, wheels, wiring, LED, resistor and switch not finalised yet |
| 5 Power budget | Written, several inputs are assumptions (see the file) |
| 6 Electronics | Written |
| 7 Wireless diagnostics | Written (no code required) |
| 8 Maze algorithms | Written |
| 9 LMS implementation | Code written and validated on sample inputs; compiles in the Arduino IDE (Verify only) but is not tested on a physical robot |

Values in the documents that depend on the final BOM are marked `TODO-BOM` and are to be updated when the BOM is complete.

## How to run

**Task 1 (desktop):**
```bash
g++ src/task1_basic.cpp -o task1 && ./task1
```

**Task 9 control logic on sample inputs (desktop):**
```bash
g++ -std=c++11 src/test/test_pid.cpp -o test_pid && ./test_pid
```
The output lists, for each sample step, the error and the left and right motor commands, including a lost-line sequence that ends in a safe stop.

**Task 9 on the ESP32:**
1. Install the ESP32 Arduino core 3.x (the sketch uses `analogWrite()`).
2. Open `src/lms_esp32/lms_esp32.ino` in the Arduino IDE (`lms_core.h` must stay in the same folder).
3. Select an ESP32 Dev Module board and upload.
4. At power-up the robot calibrates for 3 seconds: move it across the line and the floor. Serial output at 115200 baud shows sensor values, the error and the motor commands every loop.

Build note (macOS): if `g++` fails with a "tapi error ... malformed file" linker message, point the compiler at a working SDK, for example `export SDKROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX26.5.sdk`.

## Assumptions

- Sensors give higher readings over the dark line and are normalised to 0..1000 by min-max calibration at start-up.
- Control loop runs at 100 Hz (10 ms); `dt` is measured each loop.
- Starting gains (Kp 35, Ki 0, Kd 1.5, base speed 150) are initial values and need tuning on the real robot.
- Several currents in the power budget (ESP32, IR modules, encoders, buck efficiency, typical motor current) are assumptions to be measured on the robot.

## Differences from Appendix A (deliberate)

- **IN4 moved from GPIO 11 to GPIO 4.** GPIO 6 to 11 are connected to the on-module SPI flash on the ESP32-WROOM-32 and cannot be used as general I/O.
- **L298N logic supply is 5 V, not 3.3 V.** The L298 logic supply (VSS) must be 4.5 to 7 V; 3.3 V is used only as the signal level from the ESP32.
- **GPIO 25 is on ADC2, not ADC1.** ADC2 cannot be read while Wi-Fi is active, which is one reason BLE is chosen for wireless diagnostics (Task 7).
- GPIO 12 (IN3) is an ESP32 strapping pin; if boot problems occur it should be moved to another free pin.

Pin facts about the ESP32 should be confirmed against the Espressif documentation.

## Limitations

- No physical robot testing yet; all timing, thresholds and gains are untested starting values.
- The maze exploration (DFS) is documented with pseudocode and a hand-worked example but is not implemented in the sketch; junction detection is not implemented.
- The derivative term can spike when the line is lost and reacquired; the code resets the previous error on reacquisition to limit this.
- BOM prices and some part-specific limits (for example the L298N module's on-board regulator) are not yet verified.

## References

- STMicroelectronics, L298 dual full-bridge driver datasheet (DS0218).
- Espressif, ESP32 datasheet and technical reference (pin functions, strapping pins, ADC).
- Robu.in product pages for the BOM parts (links in `BOM/bom.md`).
- Comparable 12 V N20 encoder gear motor datasheet (Waveshare DCGM-N20-12V-EN-200RPM), used only as an indicative source for stall current until the chosen motor's own figure is found.
## Competition adaptation (Techfest Meshmerize)

`docs/competition_adaptation.md` lists how the design changes for the official Techfest 2026-27 Meshmerize rules. Appendix A behaviour stays the default. Added in code: a `LINE_IS_HIGH_RAW` option for a white line on a dark floor, and end-box detection with a red LED on GPIO 23 (`allSensorsOnLine` in `lms_core.h`).

To test the end-box helper on the desktop, run: g++ -std=c++11 src/test/test_end_box.cpp -o test_end_box && ./test_end_box

None of the competition changes are tested on a physical robot.
