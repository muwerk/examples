#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_presstemp_bmp180.h"
#include "mup_oled.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

ustd::PressTempBMP180 bmp("myBMP180", ustd::PressTempBMP180::FilterMode::FAST);
ustd::SensorDisplay display(128,64,0x3c);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    display.begin("myBMP180/sensor/temperature", "hastates/sensor/balkon_temperature/state", "myBMP180/sensor/pressureNN", "Studio C", "Balkon C", "Luftdruck hPa");
    display.updateDisplay("init","init","init",0.0,0.0,0.0);

    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    // sensors start measuring pressure and temperature
    bmp.setReferenceAltitude(518.0); // 518m above NN, now we also receive PressureNN values for sea level.
    bmp.startRelativeAltitude(); // Use next pressureNN measurement as altitude reference
    bmp.begin(&sched, ustd::PressTempBMP180::BMPSampleMode::ULTRA_HIGH_RESOLUTION);

    // subscribe to kernel's MQTT messages, the sensorUpdates() funktion does the event handling
    // Interal BMP180 sensor
    auto fnall = [=](String topic, String msg, String originator) {
            display.sensorUpdates(topic, msg, originator);
        };
    sched.subscribe(tID, "myBMP180/sensor/temperature", fnall);
    sched.subscribe(tID, "myBMP180/sensor/pressureNN", fnall);
    // Get outside temp via homeassistant mqtt:
    mqtt.addSubscription(tID,"hastates/sensor/balkon_temperature/state",fnall);
}

void appLoop() {
    // If a sensors doesn't update values for 1/2 hr (1800sec), declare invalid.
    if (time(nullptr)-display.lastUpdate_t1 > 1800) {
        display.t1_val=0;
    }
    if (time(nullptr)-display.lastUpdate_t2 > 1800) {
        display.t2_val=0;
    }
    if (time(nullptr)-display.lastUpdate_t3 > 1800) {
        display.t3_val=0;
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}