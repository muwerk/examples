SerialBridge example
====================

**Note:** The `muserial` protocol is not released and under heavy modification.

The muwerk serial protocol uses the `munet` `muserial.h` module to make MQTT available to devices that are
not networked using a serial bridge.

<img src="https://github.com/muwerk/examples/blob/master/Resources/SerialBridge.jpg" width="50%" height="30%">
Hardware: Adafruit Feather32 ESP32, 5V <-> 3.3V logic level shifter, Arduino Mega 2560.

The MCUs used are just examples, you can of course use different modules. Just adapt `platform.ini` and the
platform defines in `src/switch.cpp` accordingly.

## Building with platformio

* In data directory, copy `net-default.json` to `net.json` and `mqtt-default.json` to `mqtt.json`
* Customize `net.json` and `mqtt.json`, used by ESP32 to connect to the network and MQTT.
* Customoze `platform.ini` and adapt `upload_port` and `monitor_port` for both build targets.
* Build filesystem: `pio run -t buildfs -e feather32` for ESP32
* Upload filesystem: `pio run -t uploadfs -e feather32` for ESP32
* Upload program code: `pio run -t upload`, this uploads to both Arduino and ESP32.

## Documentation

* [muwerk scheduler and messaging](https://github.com/muwerk/muwerk)
* [munet networking and mqtt](https://github.com/muwerk/munet)
* [documentation of hardware mupplets and supported MQTT messages](https://github.com/muwerk/mupplet-core)
