# PB600 Sound setup page

Single-page web app to set up the sound module over USB, running in Chrome or Edge on a computer (Web Serial).
Live version (GitHub Pages): **https://oe2atn.github.io/PB600-sound/web-setup/**

- connect to the module, show live receiver values
- set volumes, maximum rpm, rev up / rev down
- calibrate the throttle end points
- save to the module, reload, factory settings
- install firmware (update or full install) with [esptool-js](https://github.com/espressif/esptool-js) by Espressif (Apache 2.0, `esptool-bundle.js`, licence in `esptool-LICENSE`)
- diagnostic log
- German and English (`?lang=en`)

The page loads the firmware from `../firmware/version.json` and the files named there.
In this repository that is `firmware/version.json` → `firmware/bin/…`, so the page also works from GitHub Pages
(Settings → Pages → deploy from branch `main`, folder `/ (root)`), at the address above.
