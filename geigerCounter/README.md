Frequency counter / Geiger counter
==================================

Using the frequency counter mupplet, a seven segment display mupplet and
a geiger counter.

<img src="https://github.com/muwerk/examples/blob/master/Resources/FrequencyCounter.jpg" width="50%">
Hardware: 2x 4.7kΩ resistor, 1nF capacitor, N-Channel mosfet (e.g. N7000),
geiger counter, 8 digit 7 segment display, Wemos D1 mini.

### Note on interface between geiger counter and ESP8266

⚠️ Make sure to check the signal (voltage levels) of the geiger counter output, and use some means to
make 3.3V logic level compatible! Different geiger counters seems to output a wide variety of different
signal levels. 

This example converts the logic level of a device that outputs levels between 5V (high) and 3.8V (low) using
a N-channel mosfet, 2x 4.7k restistors and a decoupling capacitor of 1nF.

### Building

Copy `data/net-default.json` to `net.json` and `mqtt-default.json` to `mqtt.json` and edit
the files for your environment.

Using platformio:

```
pio run -t buildfs
pio run -t uploadfs
pio run -t upload
```

### Messages sent

The example publishes to MQTT in style (hostname of ESP8266 in example was `geiger-test`):

```
omu/geiger-test/geiger/sensor/frequency 0.650
omu/geiger-test/geiger/sensor/frequency 0.600
omu/geiger-test/geiger/sensor/frequency 0.554
omu/geiger-test/geiger/sensor/frequency 0.511
omu/geiger-test/geiger/sensor/frequency 0.472
omu/geiger-test/geiger/sensor/frequency 0.435
omu/geiger-test/geiger/sensor/frequency 0.402
omu/geiger-test/geiger/sensor/frequency 0.448
omu/geiger-test/geiger/sensor/frequency 0.413
```

Output is in geiger counter events per second (cps).