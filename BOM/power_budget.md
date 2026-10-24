# Task 5: Power Budget and Battery Selection

Items marked TODO-BOM depend on the final BOM and are to be confirmed later.

## 1. Current draw per component

| Component | Rail | Idle (A) | Peak (A) | Source / assumption |
|-----------|------|----------|----------|---------------------|
| ESP32 DevKit (incl. USB-UART chip, 3.3 V regulator) | 5 V | 0.080 | 0.240 | Assumption: CPU running, radio off at idle; peak is a radio transmit burst (BLE/Wi-Fi). Check ESP32 datasheet. |
| 5 x TCRT5000 IR modules | 3.3 V (from ESP32 3V3, so counts on 5 V rail) | 0.075 | 0.075 | Assumption: about 15 mA per module (emitter LED always on). TODO-BOM: confirm on module page. |
| 2 x motor encoders | 3.3 V (same note) | 0.010 | 0.010 | Assumption: about 5 mA each. TODO-BOM. |
| L298N logic (VSS) | 5 V | 0.013 | 0.050 | ST L298 datasheet: 13 mA typical quiescent; some listings give up to 50 mA, used as peak. |
| 2 x N20 12 V motors | 11.1 V battery via L298N | 0 | 2.2 (stall, 2 x 1.1) | No-load about 0.1 A each; stall about 1.1 A each from a comparable 12 V N20 motor. TODO-BOM: confirm on the chosen motor. |

**5 V rail total:** idle = 0.080 + 0.075 + 0.010 + 0.013 = 0.178 A; peak = 0.240 + 0.075 + 0.010 + 0.050 = 0.375 A.

**Conversion to battery side** (buck converter, assumed 88 % efficient):
`I_battery = 5 V x I_5V / (0.88 x 11.1 V)` which is about 0.512 x I_5V.
- Idle: 0.178 A on 5 V gives about 0.091 A from the battery.
- Peak: 0.375 A on 5 V gives about 0.192 A from the battery.

## 2. Totals from the battery

| Case | Electronics | Motors | Total from battery |
|------|-------------|--------|--------------------|
| Idle (motors stopped) | 0.091 A | 0 | **about 0.09 A** |
| Typical running | 0.091 A | 0.6 A (assumed 0.3 A per motor, about 3 x no-load) | **about 0.69 A** |
| Worst case (both motors stalled, radio transmitting) | 0.192 A | 2.2 A | **about 2.4 A** |

The typical motor current of 0.3 A per motor is an assumption and must be measured on the real robot.

## 3. Runtime estimate

Battery: 3S LiPo, 11.1 V nominal, 3300 mAh, 25C (as listed in the BOM, TODO-BOM: confirm the model and weight).

Assumptions:
- Only 80 % of the rated capacity is used (2640 mAh) to avoid deep discharge, which damages LiPo cells.
- Constant current draw at the values above; real discharge varies with PWM duty and battery voltage sag.

| Case | Current | Runtime = 2.64 Ah / I |
|------|---------|-----------------------|
| Idle | 0.091 A | about 29 h |
| Typical running | 0.69 A | **about 3.8 h** |
| Worst case (stall) | 2.4 A | about 1.1 h (a stall would not last this long; shown as a lower bound) |

**C-rating check:** maximum continuous discharge is 25 x 3.3 A = 82.5 A, against a worst-case need of about 2.4 A. The battery has far more current capability than needed, which means a smaller (lighter, cheaper) pack would be enough. The 3300 mAh pack is justified only by long runtime; weight and cost are the trade-off (TODO-BOM: weight and price of the chosen pack, and whether a smaller pack, for example about 800 mAh, is a better fit for the chassis).

## 4. Chemistry comparison: LiPo vs Li-ion (18650 cells)

Typical qualitative values; exact figures depend on the product and must be taken from the supplier page (TODO-BOM).

| | LiPo (pouch pack) | Li-ion (18650 cylindrical) |
|---|---|---|
| Energy density | Good | Generally higher per cell |
| Discharge capability (C-rating) | High, 20C and above is common | Lower; high-drain cells exist but are usually rated at a few C |
| Weight / shape | Light, flat, can be shaped to the chassis | Heavier (steel can plus holder), fixed cylindrical shape |
| Cost | Moderate per pack | Often cheaper per Wh; needs a holder and a protection board |
| Safety | Needs a balance charger, care with puncture and over-discharge | More robust cell, but a 3S pack still needs a BMS and balanced charging |

**Choice: 3S LiPo.**
- The spec sheet (Appendix A) already assumes an 11.1 V 3S LiPo supply.
- Low weight and a flat shape suit a small robot, where weight affects acceleration and turning.
- The high C-rating is not a binding constraint here (worst case about 2.4 A), but it removes any risk of voltage sag when both motors accelerate.
- Honest trade-off: for this current level a 3S Li-ion pack would also work, and it would be cheaper per Wh. LiPo is chosen for weight and form factor, and because it is the specified supply.

## 5. Assumptions and limitations

- Several currents (ESP32, IR modules, encoders, buck efficiency, typical motor current) are assumptions and not measurements; the plan is to measure battery current with a multimeter in series on the real robot and update the table.
- Voltage sag and the L298N drop (about 2.5 V at 1 A) reduce motor power but do not change the battery current figure; they only increase the wasted heat.
- Runtime is a simple capacity divided by current estimate and ignores battery ageing and temperature.