# Bill of Materials (budget: Rs 10,000)

| # | Component | Exact model | Qty | Unit price (Rs) | Supplier link | Key specs | Why this |
|---|-----------|-------------|-----|-----------------|---------------|-----------|----------|
| 1 | Controller | ESP32 DevKit V1 30-pin (WROOM-32) | 1 | ₹434 | https://robu.in/product/esp-wroom-32-wifi-bluetooth-networking-smart-component-development-board/ | dual core, WiFi+BT, 12-bit ADC | Appendix A platform |
| 2 | Motor driver | L298N dual H-bridge module | 1 | ₹479.00 (incl. GST) | https://robu.in/product/smartelex-2x2a-dc-dual-motor-controller-v2-l298n/ | 5-35 V, 2 A peak/channel | Appendix A driver |
| 3 | IR sensors | TCRT5000 IR Tracking Sensor Module | 5 | ₹49 | https://robu.in/product/tcrt5000-ir-reflex-tracking-sensor-module/ | analog out, ___ | ___ |
| 4 | Motors | N20-12V-600 Rpm Micro Metal Gear Motor | 2 | ₹229 |https://robu.in/product/n20-12v-600-rpm-micro-metal-gear-motor/| 12V, 600 RPM | ___ |
| 5 | Battery | 3S LiPo 3300 mAh 35 C | 1 | ₹2559 | https://robu.in/product/orange-3300mah-3s-35c-80c-lithium-polymer-battery-pack-lipo/ | 11.1 V nominal | ___ |
| 6 | Charger | 3S balance charger | 1 | ₹349 | https://robu.in/product/b3ac-compact-balance-charger-2s-3s-lipo/ | ___ | ___ |
| 7 | Buck converter | ___ | 1 | ___ | ___ | ___ | ___ |
| 8 | Chassis | ___ | 1 | ___ | ___ | ___ | ___ |
| 9 | Wheels + caster | ___ | ___ | ___ | ___ | ___ | ___ |
| 10 | Wiring/connectors/switch | ___ | ___ | ___ | ___ | ___ | ___ |
| | Shipping | | | ___ | | | |
| | Contingency (10%) | | | ___ | | | |
| | **Total** | | | **___** | | | |
## Additional items required by the competition rules

| # | Component | Exact model | Qty | Unit price (Rs) | Supplier link | Key specs | Why this |
|---|-----------|-------------|-----|-----------------|---------------|-----------|----------|
| 11 | Red LED | 5 mm red LED | 1 | TODO-BOM | TODO-BOM | forward voltage about 2 V | Rule: a red LED must glow at the end zone |
| 12 | Resistor | 220 ohm, 1/4 W | 1 | TODO-BOM | TODO-BOM | limits the LED current to about 6 mA from a 3.3 V pin | Protects the GPIO and the LED |

Rule check notes:
- The chassis (row 8) is custom-made, not a ready-made kit. The 2WD kits on the supplier site include their own 3 to 6 V motors and are ready-made mechanisms, which the rules do not allow.
- Highest voltage in the robot is 12.6 V (3S LiPo full), below the 24 V limit.
- One on-board start switch (row 10).
- Add rows 11 and 12 to the totals when the prices are filled in.
