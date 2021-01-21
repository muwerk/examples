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

#ifdef __ESP32__
ustd::Led led("myLed", 14, false, 0);
// ustd::Switch toggleswitch("mySwitch",32, ustd::Switch::Mode::Default, false);
// Optional IRQ support: (each switch needs unique interruptIndex [0..9])
// ustd::Switch toggleswitch("mySwitch", 32, ustd::Switch::Mode::Flipflop, false,
//                          "mySwitch/switch/IRQ/0", 0, 25);
#else
ustd::Led led("myLed", D5, false);
// ustd::Switch toggleswitch("mySwitch", D6, ustd::Switch::Mode::Default, false);
// Optional IRQ support: (each switch needs unique interruptIndex [0..9])
// ustd::Switch toggleswitch("mySwitch",D6, ustd::Switch::Mode::Flipflop, false,
// "mySwitch/switch/IRQ/0", 0, 25);
#endif

/*
void switch_messages(String topic, String msg, String originator) {
#ifdef USE_SERIAL_DBG
    Serial.println("Switch received: " + topic + "|" + msg);
#endif
    if (topic == "mySwitch/switch/state") {
        if (msg == "on") {
            led.set(true);
            // sched.publish("myLed/light/set","on");
        } else if (msg == "off") {
            led.set(false);
            // sched.publish("myLed/light/set","off");
        } else if (msg == "trigger") {
            led.setMode(ustd::Led::Mode::Pulse, 50);
        }
    }
}
*/

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    /*int tID = */ sched.add(appLoop, "main", 1000000);
    led.begin(&sched);
    //    toggleswitch.begin(&sched);

    led.setMode(led.Mode::Wave, 1000);
    // sched.subscribe(tID, "mySwitch/switch/state", switch_messages);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}