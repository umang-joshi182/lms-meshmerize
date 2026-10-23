# Task 3 & 9: PID Line Following, Implementation and Tuning Notes

Code: `src/lms_esp32/lms_core.h` (control logic), `src/lms_esp32/lms_esp32.ino` (ESP32 sketch), `src/test/test_pid.cpp` (desktop test with hard-coded sensor samples).

## 1. Role of P, I and D

Error is the line position from the centroid (-2 to +2, 0 = line under the centre sensor, positive = line to the right).

- **P (proportional)**: correction proportional to the current error. Larger offset gives a harder steer. Too low a gain means slow, lazy turns; too high a gain means oscillation.
- **I (integral)**: accumulates error over time and removes a constant steady-state offset. On a line follower the error keeps changing sign, so the integral rarely builds up usefully, and during a long curve it can wind up and make the robot react late.
- **D (derivative)**: correction proportional to the rate of change of error. It acts like a brake: if the robot is closing in on the line quickly, D reduces the steering before it overshoots. This damps the zig-zag.

## 2. Chosen configuration: PD (Ki = 0)

- P alone oscillates around the line because it keeps steering hard until the error is exactly zero, and then overshoots.
- D damps that oscillation, which matters most at higher speeds and sharp maze turns.
- I is set to 0 by default because (a) there is no constant disturbance to remove on a flat surface, and (b) windup during turns hurts more than it helps. The code still implements I with an anti-windup clamp (`integralLimit`) so it can be enabled for a small value if a persistent drift is observed (for example one motor weaker than the other).

## 3. From sensors to motor commands

1. Read the 5 analog IR sensors and normalise each to 0..1000 using the min/max recorded during start-up calibration (higher = darker = line).
2. Weighted centroid with weights [-2, -1, 0, 1, 2]: `position = sum(reading_i * weight_i) / sum(reading_i)`. Division is guarded: if the total reading is below `minTotal`, the line is declared lost.
3. `error = position` (setpoint 0).
4. `correction = Kp*error + Ki*integral + Kd*(error - prevError)/dt`.
5. Differential drive: `left = base + correction`, `right = base - correction`. When the line is to the right (positive error) the left wheel speeds up and the right slows, so the robot turns right, towards the line.
6. Both commands are clamped to [-255, 255] with the sign preserved, then sent to the L298N (sign gives direction pins, magnitude gives PWM).

```
every 10 ms:
    s = read_and_normalise_sensors()
    position, found = weighted_centroid(s)
    if not found: handle_lost_line(); continue
    error = position
    integral = clamp(integral + error*dt, -limit, +limit)
    derivative = (error - prev_error) / dt
    correction = Kp*error + Ki*integral + Kd*derivative
    left  = clamp(base + correction)
    right = clamp(base - correction)
    prev_error = error
```

## 4. Oscillation and lost line

**Oscillation (robot zig-zags):** reduce Kp or increase Kd; if it is still unstable, reduce base speed. A very high Kd amplifies sensor noise, so lower it if the motors jitter.

**Lost line (all sensors below threshold):**
- Remember the side where the line was last seen (`lastPosition`).
- Pivot in place towards that side at `recoverSpeed`. The line most likely left the sensor array on that side (a sharp turn or an overshoot), so searching there is the fastest way to reacquire it. Spinning without that memory would be a 50/50 guess and could turn away from the line.
- Reset the integral so old error does not push the robot after reacquisition, and reset `prevError` on reacquisition to avoid a derivative spike.
- If the line is not found within `lostLimit` loops (50 loops, about 0.5 s), stop both motors. A bounded search is a safe failure mode: a robot that keeps spinning forever could drive off the track or drain the battery.

## 5. Tuning choices (Task 9)

| Parameter | Value | Reason |
|-----------|-------|--------|
| baseSpeed | 150 / 255 | About 60% duty leaves headroom for differential correction before clipping. Start slow, raise later. |
| Kp | 35 | Error range is only +/-2, so Kp is large in PWM units: an error of 1 gives a correction of 35. |
| Kd | 1.5 | With dt = 0.01 s, a change of 0.1 in error per loop gives about 15 PWM units of damping. |
| Ki | 0 | See section 2. |
| integralLimit | 50 | Anti-windup clamp. |
| recoverSpeed | 120 | Slow enough for sensors to see the line while pivoting. |
| lostLimit | 50 loops | About 0.5 s of search at 100 Hz. |
| minTotal | 200 | Ignores noise-level readings after normalisation. |
| Loop period | 10 ms (100 Hz) | Fast enough for the derivative to be meaningful; dt is measured, not assumed. |

**Procedure on the real robot (recommended order):**
1. Set Ki = 0, Kd = 0, low base speed. Raise Kp until the robot follows the line but starts to wobble, then back off by about 30%.
2. Raise Kd until the wobble disappears. Stop if the motors start to jitter.
3. Raise base speed in small steps and re-tune Kp and Kd.
4. Only if a steady drift remains, add a very small Ki.

## 6. Validation

`src/test/test_pid.cpp` feeds hard-coded sensor arrays (centred, slightly right, line on the left, lost line, reacquired) through `lineFollowStep`. Checked by hand for the sample `{0,0,3000,1000,0}`: position = 0.25, P = 8.75, D = 37.5, correction = 46.25, giving left 196 and right 104, which matches the program output. Left and right clamping to 255 and the safe stop after 3 lost steps (test uses lostLimit 3) were also observed.

## 7. Assumptions and limitations

- Not yet tested on a physical robot; all gains are starting values, not tuned values.
- Appendix A lists IN4 on GPIO 11, but GPIO 6 to 11 are connected to the on-module SPI flash on the ESP32-WROOM-32 and are not usable as I/O. IN4 was remapped to GPIO 4 in the sketch.
- Appendix A labels GPIO 25 as ADC1, but it belongs to ADC2, which cannot be used while Wi-Fi is active. This matters only if wireless diagnostics (Task 7) is added to the same sketch.
- `analogWrite()` requires ESP32 Arduino core 3.x.
- The derivative is computed on the error and can spike when the line is lost and reacquired; this is handled by resetting `prevError` on reacquisition.