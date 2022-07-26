#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"
#include "jsonfile.h"

#include "console.h"
#include "mup_oled.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
ustd::jsonfile jf;

ustd::SensorDisplay display(128,64,0x3c);

uint8_t current_slots;

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Starting up...");
#endif  // USE_SERIAL_DBG
    ustd::array<String> topics; 
    ustd::array<String> captions;
    ustd::array<String> msgs;
    ustd::array<double> dirs={0,0,0,0};
    for (uint8_t i=0; i<current_slots; i++) {
        msgs[i]="init";
        captions[i]="room";
        topics[i]="some/topic";
    }
    jf.readStringArray("display/topics", topics);
    jf.readStringArray("display/captions", captions);
    current_slots=topics.length();
    display.begin(current_slots, topics, captions);
    display.updateDisplay(msgs,dirs);

    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    auto fnall = [=](String topic, String msg, String originator) {
            display.sensorUpdates(topic, msg, originator);
        };
    for (uint8_t i=0; i<current_slots; i++) {
#ifdef USE_SERIAL_DBG
        Serial.print("Subscribing: ");
        Serial.println(topics[i]);
#endif
        mqtt.addSubscription(tID, topics[i], fnall);
    }
    // sched.subscribe(tID, "DHT-1/sensor/temperature", fnall);
}

void appLoop() {
    // If a sensors doesn't update values for 1/2 hr (1800sec), declare invalid.
    for (uint8_t i=0; i<current_slots; i++) {
        if (time(nullptr)-display.lastUpdates[i] > 1800) {
            display.vals_init[i]=false;
        }
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}