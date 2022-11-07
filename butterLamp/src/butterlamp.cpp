#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_illuminance_ldr.h"
#include "mup_neopixel.h"

#include "home_assistant.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

ustd::IlluminanceLdr ldr("ldr", A0, ustd::IlluminanceLdr::FilterMode::LONGTERM, false);
ustd::NeoPixel lamp("butterlamp", 15, 4, 8, NEO_GRB + NEO_KHZ800);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Starting Butterlamp");
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);

    ustd::jsonfile jf;
    bool hasLdr = jf.readBool("butterlamp/has_ldr", false);
    String friendlyName = jf.readString("butterlamp/friendly_name", "butterlamp");
    bool candleTimer = jf.readBool("butterlamp/candle_timer", false);
    bool registerHomeAssistant = jf.readBool("butterlamp/homeassistant", true);
    if (candleTimer) {
    }
    sched.add(appLoop, "main", 1000000);
    lamp.begin(&sched);
    if (hasLdr) {
        ldr.begin(&sched);
    }
    if (registerHomeAssistant) {
        ustd::HomeAssistant *pHa = new ustd::HomeAssistant(friendlyName, "MuWerk Intl.", "Buddha edition", "0.1.0");
        pHa->begin(&sched, true);  // true: auto-register devices in home-assistant
        pHa->addLight("butterlamp", friendlyName, ustd::HomeAssistant::LightRGB, "", "", lamp.getEffectList());
        if (hasLdr) {
            pHa->addSensor("ldr", "unitilluminance", "LDR-" + friendlyName, "illuminance", "[0..1]", "mdi:sun-wireless");
        }
    }
    lamp.setEffect(ustd::SpecialEffects::EffectType::ButterLamp, true);
    if (candleTimer) {
        String startTime, endTime;
        startTime = jf.readString("butterlamp/start_time", "18:00");
        endTime = jf.readString("butterlamp/end_time", "0:00");
#ifdef USE_SERIAL_DBG
        Serial.println("Candle " + friendlyName + " timer: " + startTime + " - " + endTime);
#endif  // USE_SERIAL_DBG
        lamp.setSchedule(startTime, endTime);
    }
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}