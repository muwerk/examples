#include "platform.h"
#include "scheduler.h"

#ifdef USD_FEATURE_NET
#include "net.h"
#include "mqtt.h"
#include "ota.h"
#endif

#if USTD_FEATURE_MEMORY >= USTD_FEATURE_MEM_8K
#include "console.h"
#endif

#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_2K
#include "digital_out.h"
#else
#include "led.h"
#endif

void appLoop();

#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_8K
ustd::Scheduler sched(2, 2, 2);
#else
ustd::Scheduler sched(10, 16, 32);
#endif

#if USTD_FEATURE_MEMORY >= USTD_FEATURE_MEM_8K
ustd::SerialConsole con;
#endif

#ifdef USTD_FEATURE_NET
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
#endif

#if defined(__ESP__) && !defined(__ESP32__)
ustd::Led led1("myLed1", D5, false);
ustd::Led led2("myLed2", D6, false);
#endif

#ifdef __ESP32__
ustd::Led led1("myLed1", 12, false);
ustd::Led led2("myLed2", 27, false);
#endif

#ifdef __UNO__
ustd::Led led1("myLed1", 3, false);
ustd::Led led2("myLed2", 5, false);
#endif

#ifdef __ATTINY__
ustd::DigitalOut led1("L1", 0, true);  // Pin next to Vcc
ustd::DigitalOut led2("L2", 1, true);  // Pin 2nd next to Vcc
#endif

#ifdef __STM32__
ustd::Led led1("myLed1", 10, true);  // Pin next to Vcc
ustd::Led led2("myLed2", 12, true);  // Pin 2nd next to Vcc
#endif

void setup() {
#if USTD_FEATURE_MEMORY >= USTD_FEATURE_MEM_8K
    Serial.begin(115200);
    con.begin(&sched);
#endif
#ifdef USTD_FEATURE_NET
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
#endif
    led1.begin(&sched);
    led2.begin(&sched);
#ifndef __ATTINY__
    led1.setMode(ustd::Led::Mode::Wave, 2000, 0.0);
    led2.setMode(ustd::Led::Mode::Wave, 2000, 0.5);
#endif
    sched.add(appLoop, "1", 1000000L);
}

#ifdef __ATTINY__
bool bl = false;
void appLoop() {
    bl = !bl;
    led1.set(bl);
    led2.set(!bl);
}
#else
void appLoop() {
}
#endif

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}