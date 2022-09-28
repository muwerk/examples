#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_temphum_dht.h"
#include "mup_illum_ldr.h"
#include "mup_temppress_bmp180.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

ustd::TempHumDHT dht("DHT-1", D5, 0);  // name, pin, unique interrupt-id (0..9)

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    // sensors start measuring temperature (and humidity)
    dht.begin(&sched);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}