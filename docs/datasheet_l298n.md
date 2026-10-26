# Task 2: Datasheet Literacy, L298N motor driver

Component: L298N dual H-bridge (module in the BOM). Reference: STMicroelectronics L298 datasheet (DS0218). Items marked TODO-BOM depend on the final BOM and are to be filled in later.

## 1. Parameters that constrain the design

**1. Output saturation voltage (voltage drop).**
Datasheet: at 1 A the source transistor drops 1.35 V typical (1.7 V max) and the sink transistor 1.2 V typical (1.6 V max). At 2 A the typical figures rise to 2.0 V and 1.7 V.
Current flows through one source and one sink in the same bridge, so the total drop is about 2.55 V at 1 A (typical).
Decisions:
- A 3S LiPo is 11.1 V nominal, so each motor sees roughly 8.5 V at 1 A. For this reason 12 V rated motors were chosen instead of 6 V ones: a 6 V motor would be over-driven at full duty, and a 12 V motor still gets a usable voltage after the drop.
- Heat: the loss is I x Vdrop, about 2.5 W per motor at 1 A. This is the main reason the driver gets hot, and the reason a MOSFET driver (lower drop) is the alternative considered for a later revision.

**2. Output current rating.**
Datasheet: 2 A per channel (4 A total DC).
Decision: the motors must stall below 2 A. Motor stall current: TODO-BOM (a comparable 12 V N20 gear motor lists about 1.1 A locked-rotor; to be confirmed on the chosen motor's page). Firmware also limits PWM (clampMotor, max 255) and stops both motors if the line is lost for too long, so a stalled motor is not driven indefinitely.

**3. Logic voltage levels and logic supply.**
Datasheet: input high is at least 2.3 V, input low at most 1.5 V; the logic supply VSS must be 4.5 to 7 V.
Decisions:
- The ESP32 outputs 3.3 V, which is above the 2.3 V minimum, so GPIO signals can drive IN1 to IN4 and ENA/ENB directly with no level shifter.
- VSS cannot come from the ESP32 3V3 pin. Appendix A calls the 3.3 V a "logic supply", but VSS needs 5 V, so it is fed from a separate 5 V buck converter (the same rail that powers ESP32 Vin). The 3.3 V only sets the signal level.

## 2. A condition that would damage the part

**Junction temperature above the rated range (130 C).**
The datasheet lists a junction operating range up to 130 C and includes over-temperature protection. Protection is a last resort, not a design margin, so the design keeps dissipation low:
- Dissipation estimate: P = I x Vdrop. At a typical cruising current of about 0.3 A per motor the loss is under 1 W per channel. At stall (about 1.1 A, TODO-BOM) it is about 2.8 W per channel, so stall must be brief.
- The firmware stops the motors after a lost-line timeout and the PWM is capped, so there is no sustained stall.
- A heatsink on the driver and airflow are used; temperature should be checked by touch or thermometer during a long test (TODO-BOM: heatsink on the chosen module).

**Related conditions avoided:**
- VSS above 7 V is out of range. The battery (11.1 V, 12.6 V when full) must never be connected to the VSS pin. With an on-module regulator or jumper, check the module documentation for its supply limit before connecting a fully charged 3S pack (TODO-BOM: confirm on the chosen module's page).
- The datasheet says the enable inputs should be low before the supply is switched on or off. The sketch drives both motors to zero in setup(), and the robot is powered down with the slide switch while stationary.

## 3. Assumptions and limitations

- Figures are from the ST L298 datasheet; the cheap module may use a different bridge or regulator, so module-specific limits must be checked on the supplier page.
- Dissipation numbers are estimates from typical saturation voltages, not measurements.

## 4. Update after the BOM was chosen

- Motors: N20 12 V 600 RPM. The supplier page does not list the stall current; other 12 V N20 motors list about 0.6 to 0.75 A, so the 1.1 A used above is a conservative bound and still leaves a margin below the 2 A per channel limit. The real stall current is to be measured on the bench and this file updated.
- The driver in the BOM is a SmartElex L298N module. The on-board regulator and jumper limits must be read from its product page before connecting a fully charged 3S pack.
- The comparison with the TB6612FNG is in `BOM/bom.md`.
