#include "ustd_platform.h"
#include "scheduler.h"

#include "muserial.h"
#include "console.h"

#ifdef __ESP__
#include "net.h"
#include "mqtt.h"
#include "ota.h"
#endif

#ifdef __ARDUINO__
#include "light_led_gpio.h"
#include "mup_switch.h"
#endif

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;

#if defined(__ARDUINO__)
// On Arduino Mega 2560, we use Arduino's Serial2, connected to a
// 5V<->3.3V logic level shifter
ustd::MuSerial muser("arduino", &Serial2, 115200, LED_BUILTIN);
#endif

#if defined(__ESP32__)
// On ESP32 feather, we use ESP32's Serial1 (feather's default tx, rx),
// connected to the 3.3V<->5V logic level shifter
ustd::MuSerial muser("feather32", &Serial1, 115200, LED_BUILTIN);
#endif

#ifdef __ESP__
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
#endif

#ifdef __ARDUINO__
ustd::LightLedGPIO led("l1", 2, true);
ustd::Switch switch1("s1", 3);
#endif

void setup() {
    // The Console is connected on both Arduino and ESP32 to default Serial-over-USB:
    Serial.begin(115200);
    con.begin(&sched);

    muser.begin(&sched);
#ifdef __ESP__
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
#endif
#ifdef __ARDUINO__
    led.begin(&sched);
    switch1.begin(&sched);
#endif
    sched.add(appLoop, "main", 1000000);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
