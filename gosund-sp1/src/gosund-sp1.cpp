#include "ustd_platform.h"
#include "scheduler.h"
//#include "doctor.h"

#include "net.h"
#include "mqtt.h"
#include "ota.h"
#include "jsonfile.h"

#include "home_assistant.h"

#include "mup_light.h"
#include "mup_switch.h"
#include "mup_digital_out.h"
#include "mup_power_bl0397.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
// ustd::Doctor doc;

ustd::Net net(13);  // use red led for network connection phase
ustd::Mqtt mqtt;
ustd::Ota ota;

ustd::HomeAssistant *pHa;

ustd::Light led1("Led1", 1, false, 0);   // blue
ustd::Light led2("Led2", 13, false, 1);  // red
ustd::Switch toggleswitch("Switch", 3, ustd::Switch::Mode::Flipflop, false);
ustd::DigitalOut relay("Relay", 14, true);

// XXX: moved to interrupt indices 2 and 3, after problems with 0,1!
ustd::PowerBl0937 power("Power", 4, 5, 12, 2, 3);
ustd::jsonfile jf;

void switch_messages(String topic, String msg, String originator) {
    if (topic == "Switch/switch/state") {
        if (msg == "on") {
            led1.set(true);
            relay.set(true);
            jf.writeBool("gosund/last_state", true);
        } else if (msg == "off") {
            led1.set(false);
            relay.set(false);
            jf.writeBool("gosund/last_state", false);
        }
    }
}

void setup() {
    net.begin(&sched, ustd::Net::Netmode::AP, false);  // false: Never reboot on net failure
    mqtt.begin(&sched);
    ota.begin(&sched);
    // doc.begin(&sched);

    int tID = sched.add(appLoop, "main", 1000000);

    led1.begin(&sched);
    led2.begin(&sched);

    toggleswitch.begin(&sched);

    String swMode = jf.readString("gosund/state_on_start", "last");
    bool lastState = jf.readBool("gosund/last_state", false);
    if (swMode == "on") {
        toggleswitch.setLogicalState(true);
    } else if (swMode == "off") {
        toggleswitch.setLogicalState(false);
    } else if (swMode == "last") {
        toggleswitch.setLogicalState(lastState);
    }

    relay.begin(&sched);
    power.begin(&sched);

    double VC = jf.readDouble("gosund/volt_calibration", 0.0, 1000.0, 1.0);
    double AC = jf.readDouble("gosund/amp_calibration", 0.0, 1000.0, 1.0);
    double WC = jf.readDouble("gosund/power_calibration", 0.0, 1000.0, 1.0);
    // calib. factors for power (W), voltage (V) and current (A)
    power.setUserCalibrationFactors(WC, VC, AC);

    String friendlyName;

    friendlyName = jf.readString("gosund/homeassistant_name", "Gosund SP1");
    pHa = new ustd::HomeAssistant(friendlyName, "muWerk", "Gosund SP1", "0.1.0");
    pHa->begin(&sched, true);
    String icon = jf.readString("gosund/homeassistant_icon", "mdi:power-socket-de");
    pHa->addSwitch("Switch", friendlyName, "switch", icon);
    pHa->addSensor("Power", "power", "Power", "power", "W");
    pHa->addSensor("Power", "voltage", "Voltage", "voltage", "V");
    pHa->addSensor("Power", "current", "Current", "current", "A");

    sched.subscribe(tID, "Switch/switch/state", switch_messages);
}

void appLoop() {
    // Serial.println("test");
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
