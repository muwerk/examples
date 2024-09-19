#define USE_SERIAL_DBG 1

#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "Wire.h"
#include <SPI.h>

#include "mup_rng.h"

ustd::Rng rng("rng", 17, 2, ustd::Rng::IM_CHANGE, 16, false, 25600);

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Startup");
#endif  // USE_SERIAL_DBG
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    rng.begin(&sched, 100000);
}

unsigned long oldSamples = 0;
void appLoop() {
#ifdef USE_SERIAL_DBG
    if (rng.getSampleCount() - oldSamples > 100000) {
        Serial.print(rng.getSampleCount());
        Serial.print(" ");
        oldSamples = rng.getSampleCount();
    }
#endif  // USE_SERIAL_DBG
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
