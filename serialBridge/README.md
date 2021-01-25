SerialBridge example
====================

**Note:** The `muserial` protocol is not released and under heavy modification.

The muwerk serial protocol uses the `munet` `muserial.h` module to make MQTT available to devices that are
not networked using a serial bridge.

<img src="https://github.com/muwerk/examples/blob/master/Resources/SerialBridge.jpg" width="50%" height="30%">

Hardware: Adafruit Feather32 ESP32, 5V <-> 3.3V logic level shifter, Arduino Mega 2560, led + 330 Ohm resistor, 
button.

The MCUs used are just examples, you can of course use different modules. Just adapt `platform.ini` and the
platform defines in `src/bridge.cpp` accordingly.

## System description

The ESP32 MCU is connected to the network and an extern MQTT server (as configured in `data/net.json` and
`data/mqtt.json`). Via Serial1, the ESP32 is connected to the Arduino Mega 2560's Serial2 (via a logic level
shifter to convert 3.3V levels to 5V). The Arduino is connected to a led and a switch (via Led and Switch muwerk
mupplets from `mupplets-core`).

To the outside world, the system of ESP32 and Arduino Mega 2560 appears as a single system. Both sub-systems
can publish and subscribe to MQTT, all message synchronisation is handled automatically via the serial link.

Let's assume the ESP has hostname `bridge-test` and the name of the serial link given to the Arduino is `arduino`.
The led is named `l1` and the switch is named `s1` during mupplet instantiation (see code).

## MQTT messages

Let's switch on the Arduino's Led using MQTT:

```
mosquitto_pub -h <mqtt-server-hostname> -t bridge-test/l1/light/set -m on
```

The led on the Arduino goes on!

Let's listen to messages from ESP32:

```
mosquitto_sub -h <mqtt-server-hostname> -v -t omu/bridge-test/#
```

Sending a led command again (from a second terminal) shows:
```
omu/bridge-test/l1/light/set on
omu/bridge-test/l1/light/unitbrightness 1.0
omu/bridge-test/l1/light/state on
```

Let's press the button that is connected to the Arduino:
```
omu/bridge-test/s1/switch/state on
omu/bridge-test/s1/switch/state off
```

From a second terminal, we can reconfigure both button and led:
```
mosquitto_pub -h <mqtt-server-host> -t bridge-test/l1/light/mode/set -m "wave 2000"
# The led connected to the Arduino now pulses in waves every 2000ms
mosquitto_pub -h <mqtt-server-host> -t bridge-test/s1/switch/mode/set -m flipflop
# Now the button acts as flipflop: every press/release, the state is changed only once.
```

## Process monitoring

We can use muwerk's [`mutop.py`](https://github.com/muwerk/muwerk/tree/master/Examples/mutop) tool,
to watch the processes both on the ESP32 and on the Arduino using an MQTT connection:

### The ESP32

```
$ python mutop.py <insert-mqtt-server-host> bridge-test

Connected to MQTT server <mqtt-server-host>.
Subscribing to omu/bridge-test/$SYS/stat
-------------------------------------------------------------------------
ID Task name    Schedule   Cnt    task rel time  cputime/call   late/call
-------------------------------------------------------------------------
 1 serial           60ms    33  1.830% ▏               5.42µs    627.55µs
 2 serlink           5ms   392 14.575% █▌              3.64µs    103.06µs
 3 net             100ms    20 37.388% ███▊          182.90µs      7.60µs
 4 mqtt            100ms    20 10.272% █              50.25µs      7.35µs
 5 ota              25ms    79 35.875% ███▋           44.43µs    325.10µs
 6 main              1 s     2  0.061%                 3.00µs      8.50µs
-------------------------------------------------------------------------
Free memory     237132 bytes, uptime: 00000000:46:47
CPU:  0.546%    | Δ: 2000013µs, OS: 118103µs, App: 1790303µs
-------------------------------------------------------------------------
```

Note: `serial` is the serial console of muwerk (see below), and `serlink` is
  the connection to the Arduino.

### The Arduino

Note: we append `\arduino` (the name of the MuSerial link of the Arduino)
to the hostname of the ESP32:

```
$ python mutop.py <mqtt-server-host> bridge-test/arduino

Connected to MQTT server <mqtt-server-host>.
Subscribing to omu/bridge-test/$SYS/stat
-------------------------------------------------------------------------
ID Task name    Schedule   Cnt    task rel time  cputime/call   late/call
-------------------------------------------------------------------------
 1 serial           60ms    33  2.622% ▎              17.09µs    729.33µs
 2 serlink           5ms   388 53.459% █████▍         29.64µs    165.49µs
 3 l1               50ms    40 38.062% ███▉          204.70µs    112.70µs
 4 s1               50ms    40  5.764% ▋              31.00µs    112.50µs
 5 main              1 s     2  0.093%                10.00µs     60.00µs
-------------------------------------------------------------------------
Free memory       3748 bytes, uptime: 00000000:13:09
CPU:  1.157%    | Δ: 2000004µs, OS: 75088µs, App: 1859068µs
-------------------------------------------------------------------------
```
The Arduino has the additional processes l1 (for the led) and s1 (for the
switch). Communication from and to those via mqtt is handled transparently
by MuSerial.

## The serial console

The example code instantiated a muwerk `Console` on the default `Serial` connection
of both ESP32 and Arduino Mega. That serial connection is available via the USB
connection to the computer used for programming.

After editing the correct port names in `platform.ini`, the console can be accessed
using platformio's `pio` tool via `pio device monitor -e feather32` for the ESP32
and `pio device monitor -e mega` for the Arduino (feather32 and mega are the environment
names given in `platform.ini`.

We get a prompt:

```
pio device monitor -e mega
--- Available filters and text transformations: colorize, debug, default, direct, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at http://bit.ly/pio-monitor-filters
--- Miniterm on /dev/tty.usbserial-2220  115200,8,N,1 ---
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---

muwerk> help
commands: help, pub, sub, uname, uptime, ps, info, mem
muwerk> sub #
All topics subscribed
>> s1/switch/state on
>> s1/switch/state off
muwerk>
```
The example show monitoring the messages on the Arduino mega using the serial console, the button was pressed,
and the messages `s1/switch/state on/off` are visible in the console.

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
