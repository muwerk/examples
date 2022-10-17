#define USE_SERIAL_DBG 1

#include "ustd_platform.h"
#include "scheduler.h"

#ifdef USTD_FEATURE_NETWORK
#include "net.h"
#include "mqtt.h"
#include "ota.h"
#endif

#if USTD_FEATURE_MEMORY >= USTD_FEATURE_MEM_8K
#include "console.h"
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN -1
#endif

#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_2K
#include "mup_digital_out.h"
#else
#include "mup_light.h"
#endif

#include "mupplet_core.h"

void appLoop();

#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_8K
ustd::Scheduler sched(2, 2, 2);
#else
ustd::Scheduler sched(10, 16, 32);
#endif

#if USTD_FEATURE_MEMORY >= USTD_FEATURE_MEM_8K
ustd::SerialConsole con;
#endif

#ifdef USTD_FEATURE_NETWORK
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
#endif

#if defined(__ESP__) && !defined(__ESP32__) && !defined(__ESP32_RISC__)
ustd::Light led1("myLed1", D5, false);
ustd::Light led2("myLed2", D6, false);
#endif

#ifdef __ESP32__
ustd::Light led1("myLed1", 12, false);
ustd::Light led2("myLed2", 27, false);
#endif

#ifdef __ESP32_RISC__
ustd::Light led1("myLed1", LED_BUILTIN, false);
ustd::Light led2("myLed2", 2, false);
#endif

#if defined(__UNO__)
#if defined(__NANO__)
ustd::Light led1("myLed1", LED_BUILTIN, false);
#else
ustd::Light led1("myLed1", 3, false);
ustd::Light led2("myLed2", 5, false);
#endif
#endif

#ifdef __ATMEGA__
ustd::Light led1("myLed1", 3, false);
ustd::Light led2("myLed2", 5, false);
#endif

#ifdef __ATTINY__
ustd::DigitalOut led1("L1", 0, true);  // Pin next to Vcc
ustd::DigitalOut led2("L2", 1, true);  // Pin 2nd next to Vcc
#endif

#ifdef __BLUEPILL__
ustd::Light led1("myLed1", 10, true);
ustd::Light led2("myLed2", 12, true);
#endif

#ifdef __BLACKPILL__
ustd::Light led1("myLed1", 10, true);
ustd::Light led2("myLed2", 12, true);
#endif

#ifdef __MAIXBIT__
ustd::Light led1("myLed1", 4, true);
ustd::Light led2("myLed2", 2, true);
#endif

#ifdef __FEATHER_M0__
ustd::Light led1("myLed1", 18, true);  // A2
ustd::Light led2("myLed2", 16, true);  // A4
#endif

#ifdef __NRF52__
ustd::Light led1("myLed1", 14, true);
ustd::Light led2("myLed2", 12, true);
#endif

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");
#if USTD_FEATURE_MEMORY >= USTD_FEATURE_MEM_8K
    Serial.begin(115200);
    con.begin(&sched);
#endif
#ifdef USTD_FEATURE_NETWORK
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
#endif
    led1.begin(&sched);
#ifndef __NANO__
    led2.begin(&sched);
#endif
#ifndef __ATTINY__
    led1.setMode(ustd::LightController::Mode::Wave, 2000, 0.0);
#ifndef __NANO__
    led2.setMode(ustd::LightController::Mode::Wave, 2000, 0.5);
#endif
#endif
    sched.add(appLoop, "1", 1000000L);

    /*
        Serial.println("Conv1");
        String forDisplay = ustd::HD44780Charset::toHD_ASCII("abgjpqyz");
        Serial.println("Conv2");
        String utfstring = ustd::HD44780Charset::toUtf8(forDisplay);
        Serial.println("Print1");
        Serial.println(xx);
        Serial.println("Print2");
        Serial.println(yy);
        Serial.println("Done");
    */
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