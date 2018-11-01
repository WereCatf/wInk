# wInk-library
A library for the Waveshare e-Ink - displays utilizing the Adafruit_GFX - library. For now, not very useable with low-RAM MCUs, but should work great on e.g. the ESP2866, ESP32 or many of the various STM32 MCUs.

# Supported displays (so far)
* Waveshare 2.9" black-and-white (tested) - 128 x 296 pixels, req. 4736B RAM for the pixel-data
* Waveshare 2.9" tri-colour (tested) - 128 x 296 pixels, req. 9472B RAM
* Waveshare 1.54" black-and-white (untested, ain't got one) - 200 x 200 pixels, req. 5000B RAM
* Waveshare 2.13" black-and-white (untested, ain't got one) - 128 x 250 pixels, req. 4000B RAM

# TODO
* Performance-optimizations
* ~~Move lut to flash~~
* ~tricolour- and~ tiling-support
* Add the rest of Waveshare-displays
* See about reducing RAM-requirements

![Two e-Ink-displays](/img/Wink.jpg)
