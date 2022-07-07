DHT temperature sensor
======================

A simple example demonstrating muwerk's scheduler, munet interface to WLAN, NTP, OTA and MQTT, with a DHT temperature sensors.

<img src="https://github.com/muwerk/mupplet-core/blob/master/extras/dht.png" width="50%" height="30%">
Hardware: D1 mini ESP8266, DHT22 sensor.

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
