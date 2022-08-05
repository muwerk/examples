#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_presstemp_bmp180.h"
#include "mup_presstemphum_bme280.h"
#include "mup_gfx_panel.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;


ustd::PressTempBMP180 bmp1("BMP180-1", ustd::PressTempBMP180::FilterMode::FAST);
ustd::PressTempHumBME280 bme1("BME280-1", ustd::PressTempHumBME280::FilterMode::FAST, 0x76);

ustd::GfxPanel displayOled("display_oled", ustd::GfxDrivers::DisplayType::SSD1306, 128, 64, 0x3c);
ustd::GfxPanel displayTft("display_tft", ustd::GfxDrivers::DisplayType::ST7735, 128, 128, D4, D3, -1);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    displayOled.begin(&sched, &mqtt);
    displayTft.begin(&sched, &mqtt);

    int tID = sched.add(appLoop, "main", 1000000);

    // sensors start measuring pressure and temperature
    bmp1.setReferenceAltitude(518.0); // 518m above NN, now we also receive PressureNN values for sea level.
    bme1.setReferenceAltitude(518.0); // 518m above NN, now we also receive PressureNN values for sea level.
    bmp1.startRelativeAltitude(); // Use next pressureNN measurement as altitude reference
    bme1.startRelativeAltitude(); // Use next pressureNN measurement as altitude reference
    bmp1.begin(&sched, ustd::PressTempBMP180::BMPSampleMode::ULTRA_HIGH_RESOLUTION);
    bme1.begin(&sched, ustd::PressTempHumBME280::BMESampleMode::ULTRA_HIGH_RESOLUTION);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
