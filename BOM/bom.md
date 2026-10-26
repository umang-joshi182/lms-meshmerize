# Bill of Materials (budget: Rs 10,000)

Prices are as seen on the Robu product pages when the parts were chosen and must be re-checked before ordering.

| # | Component | Exact model | Qty | Unit price (Rs) | Supplier link | Key specs | Why this |
|---|-----------|-------------|-----|-----------------|---------------|-----------|----------|
| 1 | Controller | ESP-WROOM-32 WiFi Bluetooth Development Board (Robu SKU 835800) | 1 | 434 (incl. GST) | https://robu.in/product/esp-wroom-32-wifi-bluetooth-networking-smart-component-development-board/ | ESP32-WROOM-32, dual core, WiFi and BLE, 12-bit ADC, micro-USB; pin count and USB chip to be confirmed on the page | Appendix A platform |
| 2 | Motor driver | SmartElex 2x2A DC dual motor controller V2 (L298N) | 1 | 479 (incl. GST) | https://robu.in/product/smartelex-2x2a-dc-dual-motor-controller-v2-l298n/ | L298N dual H-bridge: 2 A per channel, motor supply up to 46 V (ST datasheet); module-specific limits to be confirmed on the page | Appendix A driver; see the alternative comparison below |
| 3 | IR sensors | TCRT5000 IR reflex tracking sensor module | 5 | 49 | https://robu.in/product/tcrt5000-ir-reflex-tracking-sensor-module/ | Reflective IR sensor; analog output pin and operation from 3.3 V to be confirmed on the page | Five sensors form the array; Appendix A needs analog readings |
| 4 | Motors | N20 12 V 600 RPM micro metal gear motor | 2 | 229 | https://robu.in/product/n20-12v-600-rpm-micro-metal-gear-motor/ | 12 V, 600 RPM, 10 x 12 mm body, 3 mm D-shaft, no encoder; stall current not listed on the page (1.1 A assumed; other 12 V N20 motors list 0.6 to 0.75 A) | 12 V rated because the L298N drops about 2.5 V; about 0.9 m/s no-load with an assumed 30 mm wheel |
| 5 | Battery | 3S LiPo 3300 mAh 35C (Robu, Orange brand listing) | 1 | 2559 | https://robu.in/product/orange-3300mah-3s-35c-80c-lithium-polymer-battery-pack-lipo/ | 11.1 V nominal, 3300 mAh; C-rating, weight and connector to be confirmed on the page | Long runtime (about 3.8 h typical, see power budget); weight and cost are the trade-off |
| 6 | Charger | IMAX B3AC compact balance charger for 2S and 3S LiPo | 1 | 349 | https://robu.in/product/b3ac-compact-balance-charger-2s-3s-lipo/ | Balance charging of a 3S pack; specs to be confirmed on the page | Needed to charge the 3S pack safely; the supplier advises it for short-term use, so a better charger is an optional upgrade |
| 7 | Buck converter | DC-DC step-down buck module, fixed 5 V 5 A 25 W (Robu SKU 395957) | 1 | 199 (incl. GST) | https://robu.in/product/dc-dc-step-down-buck-converter-power-supply-module-24v-12v-9v-to-5v-5a-25w-replace-lm2596s/ | Fixed 5 V output, input up to 35 V per the page; minimum input, accuracy, size and efficiency to be confirmed | Makes the 5 V rail for ESP32 Vin and the L298N logic supply from the 11.1 V battery; a fixed output cannot be mis-set |
| 8 | Chassis | Not finalised: hard to choose one product at this stage; to be selected once the exact design is decided | 1 | TBD | TBD | TBD | Custom-made base planned (laser-cut acrylic or 3D-printed), not a ready-made kit, as the rules require |
| 9 | Wheels and caster | Not finalised: hard to choose one product at this stage; to be selected once the exact design is decided | 1 | TBD | TBD | TBD | N20-compatible wheels, N20 mounting brackets and a ball caster are planned; wheel diameter depends on the chassis design |
| 10 | Wiring and connectors | Not finalised: hard to choose one product at this stage; to be selected once the exact design is decided | 1 | TBD | TBD | TBD | Jumper wires and XT60 connectors are planned |
| 11 | Red LED | Not finalised: hard to choose one product at this stage; to be selected once the exact design is decided | 1 | TBD | TBD | TBD | Rule: a red LED must glow at the end zone; a 5 mm red LED is planned |
| 12 | Resistor | Not finalised: hard to choose one product at this stage; to be selected once the exact design is decided | 1 | TBD | TBD | TBD | About 220 ohm to limit the LED current to about 6 mA from a 3.3 V pin |
| 13 | Power / start switch | Not finalised: hard to choose one product at this stage; to be selected once the exact design is decided | 1 | TBD | TBD | TBD | Rule: the bot is started by one on-board switch; its current rating must exceed the worst-case battery current of about 2.4 A |
| | Subtotal, rows 1 to 7 (priced) | | | 4,723 | | 434 + 479 + 5 x 49 + 2 x 229 + 2559 + 349 + 199 | |
| | Shipping | | | To be added | | Depends on the final order | |
| | Contingency (10%) | | | To be added | | 10 % of the final subtotal | |
| | **Total** | | | **To be added** | | Must stay under Rs 10,000; about Rs 5,277 remains after rows 1 to 7, before shipping and contingency | |

## Rule check notes

- The chassis (row 8) is custom-made, not a ready-made kit. The 2WD kits on the supplier site include their own 3 to 6 V motors and are ready-made mechanisms, which the rules do not allow.
- Highest voltage in the robot is 12.6 V (3S LiPo full), below the 24 V limit.
- One on-board start switch (row 13). Whether a power switch counts as the start switch is to be confirmed with the organisers.

## Motor driver alternative: L298N (chosen) vs TB6612FNG

| | L298N (chosen) | TB6612FNG (alternative) |
|---|---|---|
| Output stage | Bipolar transistors | MOSFETs |
| Voltage drop at 1 A | about 2.55 V (source 1.35 V plus sink 1.2 V, typical) | about 0.5 V (upper plus lower, typical, VCC = VM = 5 V) |
| Heat at 1 A per motor | about 2.5 W | about 0.5 W |
| Current per channel | 2 A | 1.2 A average, 3.2 A peak |
| Motor supply | up to 46 V | 2.5 or 4.5 to 13.5 V recommended, 15 V absolute maximum |
| Logic supply | 4.5 to 7 V | 2.7 to 5.5 V (works with 3.3 V signals) |

**Choice: L298N.** Appendix A specifies it, its 2 A rating leaves a margin over the assumed motor stall of about 1.1 A, and its 46 V motor supply tolerates a full 3S pack (12.6 V) with no concern. The cost is the large drop, so more heat and less voltage at the motors.

**Why not TB6612FNG for now.** It would cut the heat to about a fifth and give the motors more voltage, but its 1.2 A average rating is close to the assumed stall current, its 13.5 V recommended maximum leaves only about 0.9 V of margin over a full 3S pack, and it needs wiring and code changes (STBY pin, PWM pins). The baseline keeps the L298N; the TB6612FNG is reconsidered after the real stall current is measured.

Sources: ST L298 datasheet, Toshiba TB6612FNG datasheet. Breakout modules may have their own limits.

## Power-block diagram

See `docs/wiring_and_troubleshooting.md`, section 1 (diagram and supply rails table).

## Status of this BOM

- Rows 1 to 7 are chosen and priced from Robu product pages.
- Rows 8 to 13 are not finalised: it is hard to choose one product before the exact design (chassis shape, motor mounts, wheel size) is decided. They will be filled in once the design is fixed, and shipping, contingency and the total will then be computed.
- Ready-made 2WD and line-follower kits seen on other shops were rejected: they are ready-made kits or mechanisms, and their motors do not match the 11.1 V design.
