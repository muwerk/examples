DHT temperature sensor
======================

A simple example demonstrating muwerk's scheduler, munet interface to WLAN, NTP, OTA and MQTT, with a GDK101 gamma detector.

<img src="https://github.com/muwerk/mupplet-sensor/blob/master/extras/gamma.png" width="50%" height="30%">
Hardware: D1 mini ESP8266, GDK101 sensor.

TBD.

## Building with platformio

* In data directory, copy `net-default.json` to `net.json` and `mqtt-default.json` to `mqtt.json`
* Customize `net.json` and `mqtt.json`
* Build filesystem: `pio run -t buildfs`
* Upload filesystem: `pio run -t uploadfs`
* Upload program code: `pio run -t upload`

## Documentation

* [muwerk scheduler and messaging](https://github.com/muwerk/muwerk)
* [munet networking and mqtt](https://github.com/muwerk/munet)
* [documentation of hardware mupplets and supported MQTT messages](https://github.com/muwerk/mupplet-core)
