#define __UNO__ 1
#undef __APPLE__

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

#include "led.h"
//#include "switch.h"

void appLoop();

// XXX: better default in muwerk itself!
#ifdef __UNO__
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
    /*int tID = */ sched.add(appLoop, "main", 1000000);
    led1.begin(&sched);
    led2.begin(&sched);

    led1.setMode(ustd::Led::Mode::Wave, 2000, 0.0);
    led2.setMode(ustd::Led::Mode::Wave, 2000, 0.5);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}