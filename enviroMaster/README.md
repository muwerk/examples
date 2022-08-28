EnviroMaster sensors
====================

A simple example demonstrating muwerk's scheduler, munet interface to WLAN, NTP, OTA and MQTT, with multiple environment sensors:

* GDK101 (gamma radiation)
* Geiger counter (cpm)
* TSL2561 (Illuminance)
* BME280 (Temperature, humidity, pressure)
* ELV lightning warner ("Gewitterwarner GW1"), based on AS3935
* MH-AD rain sensor

### Sensor output

```
omu/enviro-1/BME280-1/sensor/pressure  951.44
omu/enviro-1/BME280-1/sensor/pressureNN 1012.06
omu/enviro-1/BME280-1/sensor/humidity  28.35
omu/enviro-1/BME280-1/sensor/temperature  30.79
omu/enviro-1/GAMMA-1/sensor/gamma10minavg  0.112
omu/enviro-1/GAMMA-1/sensor/gamma1minavg  0.225
omu/enviro-1/GEIGER-1/sensor/frequency 3.143
omu/enviro-1/TSL2561-1/sensor/lightch0 7078.006
omu/enviro-1/TSL2561-1/sensor/irch1 3001.998
omu/enviro-1/TSL2561-1/sensor/illuminance 83.102
omu/enviro-1/TSL2561-1/sensor/unitilluminance  0.884
omu/enviro-1/RAIN-1/sensor/unitrain 0.914
omu/enviro-1/RAIN-1/sensor/rain ON
omu/enviro-1/LIGHTNING-NORMAL/switch/state on
omu/enviro-1/LIGHTNING-WARNING/switch/state on
```

TBD...

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
