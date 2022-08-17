
#define USE_OLED
#define USE_TFT
#define USE_BMP180
//#define USE_BMP280   // Can't be used together with BME280
#define USE_BME280
#define USE_TSL2561

#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#ifdef USE_BMP180
#include "mup_presstemp_bmp180.h"
#endif
#ifdef USE_BMP280
#include "mup_presstemp_bmp280.h"
#endif
#ifdef USE_BME280
#include "mup_presstemphum_bme280.h"
#endif
#ifdef USE_TSL2561
#include "mup_illuminance_tsl2561.h"
#endif

#if defined(USE_OLED) || defined(USE_TFT)
#include "mup_gfx_panel.h"
#endif

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;

#ifdef USE_BMP180
ustd::PressTempBMP180 bmp1("BMP180-1", ustd::PressTempBMP180::FilterMode::FAST);
#endif
#ifdef USE_BME280
ustd::PressTempHumBME280 bme1("BME280-1", ustd::PressTempHumBME280::FilterMode::FAST, 0x76);
#endif
#ifdef USE_TSL2561
ustd::IlluminanceTSL2561 tsl1("TSL2561-1", ustd::IlluminanceTSL2561::FilterMode::FAST, 0x39);
#endif

#ifdef USE_OLED
ustd::GfxPanel displayOled("display_oled", ustd::GfxDrivers::DisplayType::SSD1306, 128, 64, 0x3c, &Wire, "DE");
#endif  // USE_OLED

#ifdef USE_TFT
#ifdef __ESP32__
#define USE_CANVAS_MEMORY true
// pinout                                                                              CS  DC  RST
ustd::GfxPanel displayTft("display1", ustd::GfxDrivers::DisplayType::ST7735, 160, 128, 5, 16, 17, "DE");
#else  // ESP8266
#define USE_CANVAS_MEMORY false
ustd::GfxPanel displayTft("display_tft", ustd::GfxDrivers::DisplayType::ST7735, 128, 128, D4, D3, (uint8_t)-1, "DE");
#endif  // __ESP32__
#endif  // USE_TFT

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
#ifdef USE_OLED
    const char *topics1[] = {"TSL2561-1/sensor/lightch0", "TSL2561-1/sensor/irch1",
                             "TSL2561-1/sensor/unitilluminance", "TSL2561-1/sensor/illuminance"};
    const char *captions1[] = {"TSL _Ch0", "TSL _Ch1", "TSL _unit", "TSL _lux"};
    displayOled.begin(&sched, &mqtt, "ii|dg", 4, topics1, captions1);
    uint32_t oledFrames = 1000;
    displayOled.setSlotHistorySampleRateMs(2, oledFrames);
    displayOled.setSlotHistorySampleRateMs(3, oledFrames);
#endif
#ifdef USE_TFT
    const char *topics2[] = {"BME280-1/sensor/temperature", "BME280-1/sensor/temperature", "BME280-1/sensor/humidity",
                             "BME280-1/sensor/humidity", "BME280-1/sensor/deltaaltitude", "BME280-1/sensor/deltaaltitude",
                             "BME280-1/sensor/deltaaltitude"};
    const char *captions2[] = {"BE28 _C", "BE28 _C", "BE28 _%rH", "BE28 _%rH", "BE28 _hPa", "BE28 _hPa", "BE28 _m"};
    displayTft.begin(&sched, &mqtt, "fg|fg|dg|G", 7, topics2, captions2, USE_CANVAS_MEMORY);  // Only use canvas memory if ESP32 (large memory requirement!)
    uint32_t bmeFrames = 400;
    displayTft.setSlotHistorySampleRateMs(4, bmeFrames);  // show altitude changes every n ms
    displayTft.setSlotHistorySampleRateMs(5, bmeFrames);  // show altitude changes every n ms
    displayTft.setSlotHistorySampleRateMs(6, bmeFrames);  // show altitude changes every n ms
#endif

    int tID = sched.add(appLoop, "main", 1000000);
    // sensors start measuring pressure and temperature
#ifdef USE_BMP180
    bmp1.setReferenceAltitude(518.0);  // 518m above NN, now we also receive PressureNN values for sea level.
    bmp1.startRelativeAltitude();      // Use next pressureNN measurement as altitude reference
    bmp1.begin(&sched, &Wire, 2000, ustd::PressTempBMP180::BMPSampleMode::ULTRA_HIGH_RESOLUTION);
#endif
#ifdef USE_BME280
    bme1.setReferenceAltitude(518.0);  // 518m above NN, now we also receive PressureNN values for sea level.
    bme1.startRelativeAltitude();      // Use next pressureNN measurement as altitude reference
    bme1.begin(&sched, &Wire, bmeFrames, ustd::PressTempHumBME280::BMESampleMode::STANDARD);
#endif
#ifdef USE_TSL2561
    tsl1.begin(&sched, &Wire, oledFrames);
#endif
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
