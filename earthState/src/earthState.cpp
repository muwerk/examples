#define USE_SERIAL_DBG 1

#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "Wire.h"
#include <SPI.h>

#include "mup_rng.h"
#include "mup_magnetic_hmc5883l.h"

// Send mqtt message with topic <esp-hostname>/RNG-1/rng/data/get with empty message body to receive a block of hex random data
// Send mqtt message with topic <esp-hostname>/RNG-1/rng/state/get with empty message body to receive the state of the RNG
// <esp-hostname> is configured in data/net.json, in the field "hostname". This also contains the wifi credentials.
// The MQTT server is configured in data/mqtt.json, in the field "host".
// Copy data/net-default.json and data/mqtt-default.json to data/net.json and data/mqtt.json and adjust the fields to your needs.
// ustd::Rng rng("RNG-1", RNG_INPUT_PIN, 2, ustd::Rng::IM_CHANGE, RNG_STATE_LED_PIN, false, 25600);
ustd::MagneticFieldHMC5883L magField("MAG-1");

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    while (!Serial)
        ;  // Wait for Serial to be ready
    Serial.println("Startup");
#endif  // USE_SERIAL_DBG
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    // rng.begin(&sched, 100000);
    Serial.println("Starting magnetic field sensor");
    magField.begin(&sched, &Wire);
}

unsigned long oldSamples = 0;
void appLoop() {
#ifdef USE_SERIAL_DBG
    // if (rng.getSampleCount() - oldSamples > 100000) {
    //     Serial.print(rng.getSampleCount());
    //     Serial.print(" ");
    //     oldSamples = rng.getSampleCount();
    // }
#endif  // USE_SERIAL_DBG
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
