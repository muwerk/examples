#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "i2cdoctor.h"

#include "console.h"
#include "mup_temphum_dht.h"
#include "mup_illuminance_ldr.h"
#include "mup_presstemp_bmp180.h"
#include "mup_gfx_panel.h"
#include "home_assistant.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

// starts listening to mqtt messages to <hostname>/doctor/i2cinfo/get,
// then sends i2c used addresses, errors and device-count out.
ustd::I2CDoctor doctor("doctor");

ustd::TempHumDHT dht("DHT-1", D5, 0);  // name, pin, unique interrupt-id (0..9)
ustd::PressTempBMP180 bmp("BMP180-1");
ustd::IlluminanceLdr ldr("LDR-1", A0);
ustd::GfxPanel displayOled("display", ustd::GfxDrivers::DisplayType::SSD1306, 128, 64, 0x3c, &Wire, "DE");

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    // sensors start measuring temperature (and humidity)
    Wire.begin();
    doctor.begin(&sched, &Wire);
    dht.begin(&sched);
    bmp.begin(&sched, &Wire);
    bmp.setReferenceAltitude(520);
    ldr.begin(&sched);

    displayOled.begin(&sched, &mqtt);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}