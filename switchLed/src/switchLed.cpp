#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_light.h"
#include "mup_switch.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

ustd::Light led1("myLed1", 12, false, 0);
ustd::Light led2("myLed2", 27, false, 1);
ustd::Light led3("myLed3", 33, false, 2);
ustd::Light led4("myLed4", 15, false, 3);

ustd::Switch switch1("mySwitch1", 32, ustd::Switch::Mode::Flipflop);
ustd::Switch switch2("mySwitch2", 14, ustd::Switch::Mode::Flipflop);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    /*int tID = */ sched.add(appLoop, "main", 1000000);
    led1.begin(&sched);
    led2.begin(&sched);
    led3.begin(&sched);
    led4.begin(&sched);

    switch1.begin(&sched);
    switch2.begin(&sched);

    // generate software phase
    led1.setMode(ustd::LightController::Mode::Wave, 500, 0.0);
    // led1.setMinMaxWaveBrightness(0.02, 0.3);
    led2.setMode(ustd::LightController::Mode::Wave, 500, 0.25);
    // led2.setMinMaxWaveBrightness(0.02, 0.3);
    led3.setMode(ustd::LightController::Mode::Wave, 500, 0.5);
    // led3.setMinMaxWaveBrightness(0.02, 0.3);
    led4.setMode(ustd::LightController::Mode::Wave, 500, 0.75);
    // led4.setMinMaxWaveBrightness(0.02, 0.3);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}