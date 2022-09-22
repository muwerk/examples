#define USE_HOMEASSISTANT

#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_binary_sensor.h"
#include "mup_analog_sensor.h"

#ifdef USE_HOMEASSISTANT
#include "home_assistant.h"
#endif

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
#ifdef USE_HOMEASSISTANT
ustd::HomeAssistant ha("SwitchPotiBoard", "MuWerk Intl.", "Special Test edition", "0.0.1");
#endif
ustd::BinarySensor switch1("blackSwitch", D4, true);  // true: use inverse logic, active-low.
ustd::BinarySensor switch2("blueSwitch", D3, true);
ustd::AnalogSensor poti1("poti", A0, ustd::AnalogSensor::FilterMode::FAST);

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

    switch1.begin(&sched, 50);  // 50 ms poll rate;
    switch2.begin(&sched, 50);
    poti1.begin(&sched, 50);  // 50ms poll rate, minimal filtering;

#ifdef USE_HOMEASSISTANT
    // Register switches in same device
    ha.addBinarySensor("blackSwitch", "state", "Black Switch");
    ha.addBinarySensor("blueSwitch", "state", "Blue Switch");
    ha.addSensor("poti", "unitanalogsensor", "Potentiometer");
    // Now Homeassistant will find a new device with two binary sensors and an analog poti
    // Note: it's sometimes necessary to restart home-assistant.
#endif
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}