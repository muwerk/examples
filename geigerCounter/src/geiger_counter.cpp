#define USE_SERIAL_DBG 1

#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "Wire.h"
#include <SPI.h>

#include "mup_frequency_counter.h"
#include "display_digits_max72xx.h"

ustd::FrequencyCounter geiger("geiger", D2, 0,
                              ustd::FrequencyCounter::MeasureMode::LOWFREQUENCY_MEDIUM);
ustd::DisplayDigitsMAX72XX digits("screen", D8, 1, 1);

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

double freq;
void subMsg(String topic, String msg, String originator) {
    if (topic == "geiger/sensor/frequency") {
        freq = msg.toFloat();
    }
}

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Startup");
#endif  // USE_SERIAL_DBG
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 100000);

    geiger.begin(&sched);
    digits.begin(&sched, true);

#ifdef __ESP32__
    Wire.begin();
#endif

    sched.subscribe(tID, "geiger/sensor/frequency", subMsg);
}

String oldMsg = "";
void appLoop() {
    String msgd = String(freq) + " Hz";
    if (oldMsg != msgd) {
        sched.publish("screen/display/cmnd/format", "0;0;8;right;" + msgd);
        oldMsg = msgd;
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
