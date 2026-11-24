# Licences

## Hardware, enclosure, setup page, documentation

The contents of `hardware/`, `enclosure/`, `docs/` and the own code in `web-setup/` are licensed under
**Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)**.

You may build, copy and change them for non-commercial purposes, as long as you name the source
(PB600 Sound, github.com/oe2atn/PB600-sound) and share changes under the same licence.
Full text: https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode

## Firmware

`firmware/` is an adapted copy of [Rc_Engine_Sound_ESP32](https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32)
by TheDIYGuy999. The terms of the upstream project apply to it. The PB600 additions (`src/src/pb600Config.h`,
the `PB600_*` parts of `src/src.ino`, `src/2_Remote.h` and `src/vehicles/PistenBully600.h`) may be used under the same terms.

The PistenBully sound files (`firmware/src/vehicles/sounds/PB600*.h`) were converted from the PistenBully sound set,
publicly available from dasmikro via AliExpress, where they were obtained.

## Third-party code

`web-setup/esptool-bundle.js` is [esptool-js](https://github.com/espressif/esptool-js) by Espressif Systems,
Apache License 2.0, see `web-setup/esptool-LICENSE`.

## No warranty

Everything is provided as is, without any warranty. Building and operating the module is at your own risk.
Check the polarity of the power input before plugging in.
