Adafruit ESP32S2-TFT Billboard
==============================

![](https://github.com/domschl/python-dwd-forecast/blob/master/resources/esp32-billboard.jpg)

A simple example demonstrating muwerk's scheduler, munet interface to WLAN, 
with a TFT display that shows weather information downloaded from a [web server](https://github.com/domschl/python-dwd-forecast).

T.B.D.

## Building with platformio

* In data directory, copy `net-default.json` to `net.json`
* Customize `net.json`
* Build filesystem: `pio run -t buildfs`
* Upload filesystem: `pio run -t uploadfs`
* Upload program code: `pio run -t upload`

## Documentation

* [muwerk scheduler and messaging](https://github.com/muwerk/muwerk)
* [munet networking and mqtt](https://github.com/muwerk/munet)
* [documentation of hardware mupplets and supported MQTT messages](https://github.com/muwerk/mupplet-core)
* See [DWD weather server](https://github.com/domschl/python-dwd-forecast)
