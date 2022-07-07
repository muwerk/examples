#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_temphum_dht.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

#ifdef __ESP32__
ustd::TempHumDHT dht("myDht", 5, 0);
#else
ustd::TempHumDHT dht("myDht", D4, 0);
ustd::TempHumDHT dht2("myDht2", D7, 1);
#endif

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    /*int tID = */ sched.add(appLoop, "main", 1000000);
    dht.begin(&sched);
    dht2.begin(&sched);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}