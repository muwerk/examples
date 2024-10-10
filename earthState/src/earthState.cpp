#define USE_SERIAL_DBG 1

#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "Wire.h"
#include <SPI.h>

#include "mup_rng.h"
#include "mup_magnetic_hmc5883l.h"
#include "mup_magnetic_qmc5883l.h"

// Send mqtt message with topic <esp-hostname>/RNG-1/rng/data/get with empty message body to receive a block of hex random data
// Send mqtt message with topic <esp-hostname>/RNG-1/rng/state/get with empty message body to receive the state of the RNG
// <esp-hostname> is configured in data/net.json, in the field "hostname". This also contains the wifi credentials.
// The MQTT server is configured in data/mqtt.json, in the field "host".
// Copy data/net-default.json and data/mqtt-default.json to data/net.json and data/mqtt.json and adjust the fields to your needs.
// ustd::Rng rng("RNG-1", RNG_INPUT_PIN, 2, ustd::Rng::IM_CHANGE, RNG_STATE_LED_PIN, false, 25600);
ustd::MagneticFieldHMC5883L magField1("MAG-1");
ustd::MagneticFieldQMC5883L magField2("MAG-2");

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
    delay(2000);
    Serial.println("Startup");
#endif                       // USE_SERIAL_DBG
    WiFi.persistent(false);  // Solve possible wifi init errors (re-add at 6.2.1.16 #4044, #4083)
    WiFi.disconnect(true);   // Delete SDK wifi config
    delay(500);

    net.begin(&sched);
#if defined(MICRO_ANTENNA_REDUCED_POWER_WIFI)  // ESP32C3 Waveshare can't handle standard transmit power
                                               // due to microscopically small antenna.
                                               // Timeouts on connect happen otherwise!
#ifdef USE_SERIAL_DBG
    Serial.println("Reduced WiFi transmit power to 5dBm");
#endif
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
#endif
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    // rng.begin(&sched, 100000);
    Serial.println("Starting magnetic field sensor");
    magField1.begin(&sched, &Wire, 2000, ustd::MagneticFieldHMC5883L::HMC5883LSamples::SAMPLE_AVERAGE_1,
                    ustd::MagneticFieldHMC5883L::HMC5883LMeasurementMode::MEASUREMENT_MODE_NORMAL,
                    ustd::MagneticFieldHMC5883L::HMC5883LDataOutputRate::DATA_OUTPUT_RATE_15,
                    ustd::MagneticFieldHMC5883L::HMC5883LGain::GAIN_5_6);
    magField2.begin(&sched, &Wire, 2000);
}

unsigned long oldSamples = 0;
void appLoop() {
    double m1 = magField1.getMagneticFieldStrength();
    double m2 = magField2.getMagneticFieldStrength();
    if (m1 != 0.0 || m2 != 0.0) {
        Serial.print("Relative strength: ");
        Serial.println(m1 / m2);
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
