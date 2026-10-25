# Competition adaptation (Techfest 2026-27 Meshmerize)

Beyond the nine selection tasks. The selection work follows Appendix A; the official Meshmerize problem statement differs in places, so the differences are handled as options and notes without changing the Appendix A behaviour. None of this has been tested on a physical robot.

## 1. Rule-driven changes

| Rule in the Meshmerize statement | What was done |
|---|---|
| White vinyl line (30 mm) on a dark arena | The sketch has a `LINE_IS_HIGH_RAW` flag. true = Appendix A behaviour (line gives the higher reading). Set it to false if a bench check shows the line gives the lower reading. Calibration stays automatic min-max. Not yet checked on real sensors. |
| Red LED must glow at the end zone; the end box is a 400 x 400 mm white square | Red LED on GPIO 23 through a series resistor (see BOM). End box = all five sensors on the line for at least 250 ms (`END_BOX_MS`). A perpendicular 30 mm stripe at a junction trips all sensors only briefly, so the time condition separates the two cases. 250 ms is a starting value to be tuned. On detection the motors stop and the LED stays on. |
| Wi-Fi and Bluetooth devices must be off; no laptops near the arena | The race firmware never starts Wi-Fi or BLE. The BLE telemetry in Task 7 is a development tool only; USB serial is used at the bench. |
| Code cannot be changed after the bot is deposited; 1 minute of calibration; no touching during the run | All gains and thresholds must be fixed before depositing. Calibration is currently a 3 s manual sweep; a self-driven sweep is not implemented yet. |
| No ready-made kits or mechanisms; ready-made gear assemblies are allowed | The chassis is custom-made (acrylic or 3D-printed), not a ready-made robot kit. N20 gear motors and wheels are used as gear assemblies. To be confirmed with the organisers. |
| Fits a 220 mm cube; at most 24 V between any two points; one start switch | Target footprint well under 220 mm (to be measured). A 3S LiPo is 12.6 V when full. One on-board slide switch. |

## 2. Scoring and priorities

Total = A + B + C + S + L + T - P. A is 25 per checkpoint (the figure shows four, so up to 100), B is 30 for completing the dry run, S is 30 for the shortest path, L is 5 for the LED. That is up to 165 fixed points. C (180 minus dry-run seconds) and T (150 minus actual-run seconds) give one point per second saved. Reliability (all checkpoints, a completed dry run, the shortest path) is worth more than speed, so tuning for speed comes after the run is reliable.

## 3. Maze algorithm for this arena

The uniform-grid assumption in the Task 8 notes does not fit this arena (paths of different lengths, dead ends, junctions). See section 7 of `docs/maze_analysis.md`.

## 4. Not done yet

- Maze exploration, junction classification and dead-end U-turn are not implemented in the sketch. The lost-line recovery pivots toward the last seen side for at most 0.5 s, which is not a full U-turn.
- Automatic (self-driven) calibration.
- Tuning of gains, thresholds and the 250 ms end-box time on the real vinyl surface.
- Restart behaviour (dry run restarts from a checkpoint, actual run from the start zone).
