#undef __APPLE__
// #define USE_SERIAL_DBG 1

#include "ustd_platform.h"
#include "scheduler.h"

#include "net.h"
#include "ota.h"
#include "mqtt.h"

#include "doctor.h"
#include "console.h"
#include "display_matrix_max72xx.h"
#include "muMatrix8ptRegular.h"
#include "muHeavy8ptBold.h"

// entities for core functionality
ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Doctor doc;
ustd::Net net(LED_BUILTIN);
ustd::Ota ota;
ustd::Mqtt mqtt;
ustd::DisplayMatrixMAX72XX matrix("matrix", D8, 12, 1, 1);

// main application command handler
void app_command_handler(String topic, String msg, String originator) {
}

// main application task
void app() {
}

// application setup
void setup() {
    Serial.begin(115200);
    DBG("\nStartup: " + WiFi.macAddress());

    // extend console
    con.extend("disp", [](String cmd, String args) {
        String verb = ustd::shift(args);
        if (verb.length()) {
            sched.publish("matrix/display/" + verb, args);
        } else {
        }
    });

    String a;
    // initialize core components
    con.begin(&sched, "sub net/# mqtt/# doctor/# matrix/#", 115200);
    doc.begin(&sched);
    matrix.begin(&sched);
    matrix.addfont(&muMatrix8ptRegular, 7);
    matrix.addfont(&muHeavy8ptBold, 7);

    net.begin(&sched);
    ota.begin(&sched);
    mqtt.begin(&sched);

    // initialize application
    int tID = sched.add(app, "main", 1000000L);
    sched.subscribe(tID, "cmnd/#", app_command_handler);
}

// application runtime
void loop() {
    sched.loop();
}