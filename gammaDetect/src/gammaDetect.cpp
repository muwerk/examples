#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"

#include "ota.h"

#include "console.h"
#include "jsonfile.h"

#include "doctor.h"
#include "i2cdoctor.h"

#include "mup_gfx_panel.h"
#include "mup_gamma_gdk101.h"
#include "mup_frequency_counter.h"

void appLoop();

ustd::Scheduler sched(10,16,32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
ustd::I2CDoctor i2cdoc;

ustd::GfxPanel display("display", ustd::GfxDrivers::DisplayType::ST7735, 128, 128, D4, D3, (uint8_t)-1, "DE");
ustd::GammaGDK101 gammaG("GAMMA-1",ustd::GammaGDK101::FilterMode::FAST);
ustd::FrequencyCounter geiger(GEIGER-1", D0, 0, ustd::FrequencyCounter::MeasureMode::LOWFREQUENCY_MEDIUM);

// ustd::GfxPanel display("display", ustd::GfxDrivers::DisplayType::SSD1306, 128, 64, 0x3c);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    i2cdoc.begin(&sched, &Wire);
    display.begin(&sched, &mqtt);
    gammaG.begin(&sched);
    geiger.begin(&sched);

    int tID = sched.add(appLoop, "main", 1000000);

}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
