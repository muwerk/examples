#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"

#include "ota.h"

#include "console.h"
#include "jsonfile.h"

#include "mup_gfx_panel.h"
#include "mup_gamma_gdk101.h"
#include "mup_frequency_counter.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

#ifdef __ESP32__
ustd::FrequencyCounter geiger("GEIGER-1", 26, 2, ustd::FrequencyCounter::MeasureMode::LOWFREQUENCY_FAST);
ustd::GfxPanel display("display-b", ustd::GfxDrivers::DisplayType::ST7735, 128, 160, 5, 16, 17, "DE");
#else
ustd::GfxPanel display("display", ustd::GfxDrivers::DisplayType::ST7735, 128, 128, D4, D3, (uint8_t)-1, "DE");
#endif
ustd::GammaGDK101 gammaG("GAMMA-1", ustd::GammaGDK101::FilterMode::FAST);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    display.begin(&sched, &mqtt, true);

    gammaG.begin(&sched, &Wire, true);
    uint32_t framesMs = 250;
#ifdef __ESP32__
    geiger.begin(&sched, framesMs * 1000L);  // measure every framesMs*1000 us.
#endif
    display.setSlotHistorySampleRateMs(1, framesMs);  // Geiger counter graphics slot 1, rate update in ms.
    display.setSlotHistorySampleRateMs(0, 500);       // Geiger counter graphics slot 0, rate update in ms.
    int tID = sched.add(appLoop, "main", 1000000);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
