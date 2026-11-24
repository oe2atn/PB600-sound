# PB600 Sound Carrier, rev 1.3

Plug-on board for an ESP32 DevKit (30 pin), 28.5 × 52 mm, 2 layers. All parts sit between the header rows.

| File | Content |
|---|---|
| `production/PB600-Gerber.zip` | Gerber files for JLCPCB |
| `production/BOM_JLCPCB.csv` | bill of materials with LCSC part numbers |
| `production/CPL_JLCPCB.csv` | pick and place |
| `kicad/` | KiCad 7 project (schematic and layout) |
| `PB600-Sound-Carrier-Schematic.pdf` | schematic |
| `PB600-Sound-Carrier-Datasheet.pdf` | one-page data sheet (enclosure dimensions there are for enclosure v1.0) |
| `PB600-Sound-Carrier-Overview.pdf` | connector overview (German) |

## Key data

- Input 7 to 18 V (2S to 4S LiPo), reverse polarity protection B5819W
- AP63205 buck converter, 5 V / 2 A for the ESP32
- PAM8403 class D amplifier, bridged mono output, approx. 1.5 W into 8 Ω
- 2 × PWM inputs (THR on GPIO14, FN on GPIO27), 4.7 kΩ series resistor and BAT54C clamp to 3.3 V
- Receiver power is not used (servo plus pin not connected)

## Connectors

| Ref | Function | Type | Mating plug |
|---|---|---|---|
| J5 | power in, 7 to 18 V | JST-PH 2.0 mm, 2 pin, side entry (S2B-PH-K) | JST PHR-2 with crimp contacts, or a ready-made JST-PH 2-pin lead |
| J6 | speaker 4 to 8 Ω | JST-PH 2.0 mm, 2 pin, side entry (S2B-PH-K) | JST PHR-2 |
| J3 | THR, throttle signal | servo header 2.54 mm, 3 pin, right angle | servo lead (JR/Futaba) |
| J4 | FN, function signal | servo header 2.54 mm, 3 pin, right angle | servo lead (JR/Futaba) |
| J1, J2 | ESP32 | female header 1 × 15, 2.54 mm | ESP32 DevKit 30 pin |

> [!CAUTION]
> **J5 polarity:** pin 1 is +, pin 2 is −, marked + and − on the board. Ready-made JST-PH leads often have red and black the other way round. Measure before plugging in and swap the contacts in the plug if needed.

## Ordering

See [the guide](../docs/GUIDE.md#2-ordering-the-board-from-jlcpcb) ([deutsch](../docs/ANLEITUNG.md#2-platine-bei-jlcpcb-bestellen)).
**Check the rotation of all parts in the JLCPCB 3D preview** before ordering.
