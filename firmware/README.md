# PB600 Sound firmware

Firmware for the PB600 Sound Carrier: an adapted copy of
[Rc_Engine_Sound_ESP32](https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32) by TheDIYGuy999
(version 9.15.0-b8, October 2026) for the 4x3D Xbox receiver in PWM mode.

Current version: **1.7** (shown on the setup page and in the serial log as `PB:READY 1.7`).

## Ready-made binaries

| File | Address | Use |
|---|---|---|
| `bin/PB600-Sound-1.7-full.bin` | `0x0` | new or empty ESP32; erases all settings |
| `bin/PB600-Sound-1.7-app.bin` | `0x10000` | update; settings are kept |

Easiest: the [setup page](https://oe2atn.github.io/PB600-sound/web-setup/?lang=en) installs them with one click.
With esptool:

```bash
esptool.py --chip esp32 --baud 460800 write_flash 0x0 bin/PB600-Sound-1.7-full.bin
esptool.py --chip esp32 --baud 460800 write_flash 0x10000 bin/PB600-Sound-1.7-app.bin
```

## Building

```bash
pio run              # build
pio run -t upload    # flash
pio device monitor   # serial log, 115200 baud
```

- PlatformIO platform `espressif32@6.10.0` (newer versions do not compile).
- The libraries are fetched by PlatformIO on the first build.
- Only the vehicle and sound files needed for the PB600 are included; the full sound library is in the upstream project.

## What differs from upstream

| File | Change |
|---|---|
| `src/1_Vehicle.h` | vehicle `vehicles/PistenBully600.h` |
| `src/vehicles/PistenBully600.h` | PistenBully 600 sounds (start, idle, shutdown, horn, 2 kHz reversing beep); separate driving sound (`REV_SOUND`) off, only the idle revs up; `MAX_RPM_PERCENTAGE 130`, `acc 2`, `dec 4`; indicator sound off; `PB600_CONFIG` on |
| `src/2_Remote.h` | receiver profile `PB600_XBOX`: PWM, CH3 THROTTLE = GPIO14 (J3 THR), CH4 HORN = GPIO27 (J4 FN), auto zero only on the throttle |
| `src/src.ino` | changes active only with `PB600_CONTROLS` / `PB600_CONFIG`, see below |
| `src/src/pb600Config.h` | new: settings over USB for the setup page |
| `platformio.ini` | platform pinned to `espressif32@6.10.0` |

Changes in `src/src.ino`:

- **Unused inputs held at neutral (1500 µs).** Floating inputs triggered the indicator and caused a ticking noise.
- **rpm follows the throttle directly** (no virtual inertia); drops back to idle quickly when released.
- **Reversing beep only while reverse throttle is given**, stops at neutral.
- **Engine on/off:** FN channel below 1150 µs for 1 s while standing still toggles the engine. Giving throttle also starts it.
- **Horn:** FN channel above 1900 µs.
- **Shutdown sound** is played when the engine is switched off.
- **Headlight moved to GPIO22**, because GPIO3 is the USB receive line.
- **Upstream serial command interface off** (it swallowed the setup page commands); `pb600ConfigLoop()` runs instead.
- **Master volume** comes from the stored settings instead of the volume steps.

## USB settings protocol (`pb600Config.h`)

Line based, 115200 baud, used by the setup page via Web Serial. Answers start with `PB:`.

| Command | Answer | Meaning |
|---|---|---|
| `?` | `PB:CFG {json}` | all values with min, max, default, plus firmware version |
| `S <key> <value>` | `PB:OK <key> <value>` | set a value (applied at once, not stored) |
| `W` | `PB:SAVING`, `PB:SAVED` or `PB:ERR save <n>` | store all values in NVS |
| `R` | `PB:CFG …` | reload stored values |
| `D` | `PB:CFG …` | load defaults (not stored) |
| `C F` / `C R` | `PB:OK …` | take the current throttle pulse as forward / reverse end point |
| `L1` / `L0` | `PB:LIVE {…}` every 100 ms | live values on / off |

On start the module sends `PB:READY <version> <reset reason>`.

Keys: `master`, `idle`, `full`, `start`, `stop`, `horn`, `beep` (volumes in %), `rpm` (max rpm %),
`rise`, `fall` (acc, dec), `fwd`, `rev` (throttle end points in µs).
Stored as one blob in NVS namespace `pb600`, key `cfg`.

## Sound files

The PistenBully sound files (`src/vehicles/sounds/PB600*.h`) were converted from the PistenBully sound set,
which is publicly available from **dasmikro via AliExpress** and was obtained from there.
All other sound files come from the upstream project.
