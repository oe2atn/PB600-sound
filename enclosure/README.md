# PB600 Sound enclosure, v1.1

Screwed two-part enclosure for the carrier with ESP32. Outside 33.1 × 68.2 × 25.5 mm; only the connector openings and the USB port are open.

| File | Content |
|---|---|
| `PB600-Enclosure-Base.stl` | base, ready to print (floor down) |
| `PB600-Enclosure-Lid.stl` | lid, ready to print (top face down) |
| `PB600-Enclosure-Lid-Lettering.stl` | lettering inlay, same position as the lid |
| `PB600-Enclosure-Lid-2colour.3mf` | lid and lettering as one object with two parts |
| `pb600_case.scad` | OpenSCAD source, all dimensions at the top |

## Printing

- PLA or PETG, 0.2 mm layers, 4 walls, 20 % infill, no supports.
- Lettering in a contrasting colour (intended: black): open the 3MF, give the part "Schrift" the second colour. It sits in the first 2 layers (0.4 mm) because the lid is printed upside down.
- Single colour: print only the lid STL; the lettering stays visible as a recess.

## Assembly

- 4 × socket head screw M2 × 16 (DIN 912 / ISO 4762) from below; they cut their own thread into the lid posts, tighten gently.
- Heat-set inserts instead: set `INSERT = true` in `pb600_case.scad` (M2, 3.2 × 4 mm).

v1.1: 2 mm longer than v1.0 for thicker walls around the screw heads; larger connector labels as a two-colour inlay.
