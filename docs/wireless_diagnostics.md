# Task 7: Wireless Sensor Diagnostics

Scenario: the maze surface has small bumps and one or more IR sensors give intermittent readings. The faulty sensor(s) must be identified in real time, without stopping the robot or touching the hardware.

## 1. Protocol selection: BLE

| | Wi-Fi | Classic Bluetooth (SPP) | BLE |
|---|---|---|---|
| Throughput | Highest | Medium | Low to medium, enough here |
| Latency | Low | Low to medium | Set by connection interval, typically tens of ms |
| Power / RF burst | Highest, transmit bursts draw high current | Medium | Lowest |
| Implementation | Network setup, IP, sockets | Easiest: behaves like a serial port | GATT service and characteristic, a bit more work |
| Phone support | Android and iOS | Android and PC only, not iOS | Android, iOS and PC |
| Conflict with this design | **ADC2 cannot be used while Wi-Fi is on** | None | None |

**Required data rate:** 5 sensors x 2 bytes = 10 bytes, plus sequence number and timestamp, about 15 bytes per frame. At 100 Hz this is about 1.5 KB/s (about 12 kbit/s). Every option can carry that, so throughput is not the deciding factor.

**Decision: BLE.**
- **Hardware conflict:** the spec sheet places S5 on GPIO 25, which belongs to ADC2 on the ESP32 and cannot be read while Wi-Fi is active (to be confirmed in the Espressif ADC documentation). Using Wi-Fi would force S5 to move to an ADC1-only pin. BLE needs no change.
- **Electrical noise:** Wi-Fi transmit bursts draw current spikes from the 3.3 V rail, which can add noise to the analog readings we are trying to diagnose. A diagnostic channel should not disturb the measurement. BLE has the smaller RF load.
- **Compatibility:** BLE works with any phone and with a laptop, so the team is not tied to one device.
- **Latency is adequate:** this is one-way telemetry for diagnosis, not control, so tens of ms of delay does not matter.
- **Trade-off accepted:** BLE needs a small GATT service (one custom service, one notify characteristic), which is more setup than a serial-style link. If time is very short, Classic Bluetooth SPP is the simplest fallback on Android and PC.

**Key design rule:** the control loop must never wait for the radio. The wireless link is telemetry only.

## 2. System architecture

```
 IR sensors x5 --> ESP32 control loop (100 Hz, core 1)
                         |  raw 12-bit values + timestamp + sequence no.
                         v
                  ring buffer / queue (non-blocking, oldest dropped if full)
                         |
                         v
                 diagnostic task (core 0)
                   - per-sensor health checks
                   - pack 5 frames per BLE packet
                         |  BLE notifications (GATT notify characteristic)
                         v
                 phone / PC receiver
                   - live plot of all 5 channels
                   - CSV log with timestamps
                   - fault highlight per sensor
```

**Stage by stage:**
1. **Read:** the control loop reads the 5 sensors every 10 ms and stamps each frame with a sequence number and timestamp. It continues to drive the motors exactly as before.
2. **Buffer:** frames go into a non-blocking queue. If the radio is slow, old frames are dropped; the control loop is never delayed.
3. **Diagnostic task (second core):** computes simple health flags per sensor (below) and packs several frames into one BLE notification. Batching 5 frames of about 15 bytes each gives about 75 bytes per packet, 20 packets per second, which fits inside a negotiated MTU and reduces radio overhead.
4. **Transmit:** the phone or PC subscribes to the notify characteristic, so data is pushed without polling.
5. **Receive and visualise:** a PC script or phone app plots the 5 channels live, saves everything to CSV, and highlights the sensor whose health flag is raised.
6. **Loss detection:** BLE notifications are not acknowledged by the application, so the sequence number lets the receiver tell a lost packet apart from a real sensor dropout.

## 3. How the faulty sensor is identified

A bump changes the height of the whole array above the surface, so it disturbs **all five sensors at the same time**. A faulty sensor misbehaves **on its own channel**, whether or not there is a bump. This is the main discriminator. The health checks per channel:

- **Spike / dropout:** a reading that jumps by more than a threshold between consecutive frames while its neighbours stay steady.
- **Rail value:** a reading stuck at 0 or 4095 while the other channels vary.
- **Stuck value:** a reading that never changes over many frames even though the robot is moving over the line.
- **Rate:** the number of spikes per second for each channel. A channel with a persistently higher count than the others is the suspect; a burst on all channels at once is attributed to a bump.

Thresholds are starting guesses and must be set from logged data on the real robot.

## 4. Assumptions and limitations

- No code is required for this task, so the design is not implemented or tested on the robot.
- The ADC2 and Wi-Fi restriction and the throughput figures should be checked against the Espressif documentation before relying on them.
- Detection thresholds need calibration on logged data from the actual maze.
- Without an IMU the system cannot measure the bumps directly; it infers them from simultaneous disturbance across all channels.