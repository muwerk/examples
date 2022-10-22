#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_light.h"
#include "mup_switch.h"
#include "mup_neopixel.h"

#include "home_assistant.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
ustd::HomeAssistant ha("SwitchLedBoard", "MuWerk Intl.", "Special Test edition", "0.0.1");
ustd::Light led1("blueLed", D5, false, 0);
ustd::Light led2("yellowLed", D6, false, 1);
ustd::Light led3("greenLed", D7, false, 2);
ustd::NeoPixel pix1("neoPixel", D8, 1, NEO_RGB + NEO_KHZ400);

ustd::Switch switch1("blackSwitch", D4);  // , ustd::Switch::Mode::Flipflop);
ustd::Switch switch2("blueSwitch", D3);   // , ustd::Switch::Mode::Flipflop);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Starting switch_test");
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    ha.begin(&sched, true);  // true: auto-register devices in home-assistant

    sched.add(appLoop, "main", 1000000);

    led1.begin(&sched);
    led2.begin(&sched);
    led3.begin(&sched);
    pix1.begin(&sched);

    // register leds in home-assistant, they will all be part of device "SwitchLedBoard" as defined above.
    ha.addLight("blueLed", "Blue Led");
    ha.addLight("yellowLed", "Yellow Led");
    ha.addLight("greenLed", "Green Led");
    ha.addLight("neoPixel", "NeoPixel", ustd::HomeAssistant::LightRGB);
    pix1.color(0, 0, 255);

    switch1.begin(&sched);
    switch2.begin(&sched);

    // Register switches in same device
    ha.addSwitch("blackSwitch", "Black Switch", "switch");
    ha.addSwitch("blueSwitch", "Blue Switch", "switch");
    // Now Homeassistant will find a new device with four leds and two switches...
    // Note: it's sometimes necessary to restart home-assistant.
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}