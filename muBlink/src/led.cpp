#include "platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "led.h"
//#include "switch.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

ustd::Led led1("myLed1", D5, false);
ustd::Led led2("myLed2", D6, false);
ustd::Led led3("myLed3", D7, false);

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
    //    toggleswitch.begin(&sched);

    led1.setMode(ustd::Led::Mode::Wave, 1000, 0.0);
    led2.setMode(ustd::Led::Mode::Wave, 1000, 0.5);
    led3.setMode(ustd::Led::Mode::Blink, 500);

    // sched.subscribe(tID, "mySwitch/switch/state", switch_messages);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}