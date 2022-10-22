#define USE_SERIAL_DBG 1
#define OPTION_HOMEASSISTANT

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

//#ifndef LED_BUILTIN
//#define LED_BUILTIN (0xff)
//#endif

#include "mupplet_core.h"
#include "mup_neopixel.h"

#ifdef OPTION_HOMEASSISTANT
#include "home_assistant.h"
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

#ifdef USTD_FEATURE_NETWORK
// ustd::Net net(LED_BUILTIN);
ustd::Net net(-1);
ustd::Mqtt mqtt;
ustd::Ota ota;
#endif

#ifdef OPTION_HOMEASSISTANT
ustd::HomeAssistant ha("Esp32C3TestBoard", "MuWerk Intl.", "Special Test edition", "0.0.1");
#endif

ustd::NeoPixel pix1("pix1", 2, 1, NEO_GRB + NEO_KHZ800);

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
    pix1.begin(&sched);
#ifdef OPTION_HOMEASSISTANT
    ha.begin(&sched, true);
    ha.addLight("pix1", "c3pixel", ustd::HomeAssistant::LightRGB);
#endif
    pix1.pixel(0, 0, 0, 255);

    // seq={{0,0x010203},{1,0x020301},{-1,100},{-2,100}}
    sched.add(appLoop, "1", 1000000L);
}

bool bl = false;
void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
