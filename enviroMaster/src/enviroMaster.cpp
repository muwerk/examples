#define LIGHTNING_WARNING_STANDARD

#define USE_HOMEASSISTANT

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
#include "mup_illuminance_tsl2561.h"
#include "mup_presstemphum_bme280.h"
#include "mup_switch.h"
#include "mup_rain_ad.h"

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
ustd::HomeAssistant ha("EnviroMaster", "MuWerk Intl.", "Test-Version", "0.1.0");
#endif

ustd::GfxPanel display("display", ustd::GfxDrivers::DisplayType::ST7735, 128, 160, 5, 16, 17, "DE");
ustd::FrequencyCounter geiger("GEIGER-1", 26, 2, ustd::FrequencyCounter::MeasureMode::LOWFREQUENCY_MEDIUM);
ustd::GammaGDK101 gammaG("GAMMA-1", ustd::GammaGDK101::FilterMode::FAST);
ustd::PressTempHumBME280 bme280("BME280-1", ustd::PressTempHumBME280::FilterMode::FAST);

// Ports 32-34 connected to "ELV Gewitterwarner GW2", an AS3935 based lighning detector.
ustd::Switch lightningNone("LIGHTNING-NORMAL", 32, ustd::Switch::Mode::BinarySensor);   // Normal, no warnings
ustd::Switch lightningActive("LIGHTNING-EVENT", 33, ustd::Switch::Mode::BinarySensor);  // Active lightning!
#ifdef LIGHTNING_WARNING_STANDARD
ustd::Switch lightningWarning("LIGHTNING-WARNING", 34, ustd::Switch::Mode::BinarySensor);
#else
ustd::FrequencyCounter lightningWarning("LIGHTNING-STATIC-CHARGE", 34, 1, ustd::FrequencyCounter::MeasureMode::LOWFREQUENCY_MEDIUM);  //  a measure of electrostatic disturbances
#endif

ustd::RainAD rainAD("RAIN-1", A7, 25);

ustd::IlluminanceTSL2561 tsl2561("TSL2561-1", ustd::IlluminanceTSL2561::FilterMode::FAST, ustd::IlluminanceTSL2561::IntegrationMode::LONGTERM402ms, ustd::IlluminanceTSL2561::GainMode::LOW1x, 0x29);  // non-standard i2c address, adr-select low, otherwise clash with GDK101 which is on 0x39

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);

#ifdef USE_HOMEASSISTANT
    ha.begin(&sched, true);
#endif

    display.begin(&sched, &mqtt, true);
    uint32_t framesMs = 1000;
    uint32_t textMs = 2000;

    gammaG.begin(&sched, &Wire, framesMs, true);
    geiger.begin(&sched, framesMs * 1000L);  // measure every framesMs*1000 us.
    bme280.begin(&sched, &Wire, framesMs);   // measure every framesMs*1000 us.
    bme280.setReferenceAltitude(518.0);      // 518m above NN, now we also receive PressureNN values for sea level.

    tsl2561.begin(&sched, &Wire, framesMs);  // measure every framesMs*1000 us.

    lightningNone.begin(&sched);
    lightningActive.begin(&sched);
    lightningActive.activateCounter(true);  // send `count` messages for every detected lightning
    lightningWarning.begin(&sched);

    rainAD.begin(&sched);

    display.setSlotHistorySampleRateMs(0, textMs);    // Geiger counter graphics slot 1, rate update in ms.
    display.setSlotHistorySampleRateMs(1, framesMs);  // Geiger counter graphics slot 0, rate update in ms.
    display.setSlotHistorySampleRateMs(2, textMs);    // Geiger counter graphics slot 1, rate update in ms.
    display.setSlotHistorySampleRateMs(3, textMs);    // Geiger counter graphics slot 0, rate update in ms.
    display.setSlotHistorySampleRateMs(4, framesMs);  // Geiger counter graphics slot 1, rate update in ms.
    display.setSlotHistorySampleRateMs(5, framesMs);  // Geiger counter graphics slot 0, rate update in ms.

    sched.add(appLoop, "main", 1000000);

#ifdef USE_HOMEASSISTANT
    ha.addSensor("GEIGER-1", "frequency", "Geiger counter", "frequency", "Hz", "mdi:radioactive");
    ha.addSensor("GAMMA-1", "gamma10minavg", "Gamma 10min avg", "frequency", "Hz*60", "mdi:counter");
    ha.addSensor("GAMMA-1", "gamma1minavg", "Gamma 1min avg", "frequency", "Hz*60", "mdi:counter");
    ha.addSensor("BME280-1", "temperature", "Temperature", "temperature", "°C");
    ha.addSensor("BME280-1", "pressureNN", "Pressure NN", "pressure", "hPa");
    ha.addSensor("BME280-1", "pressure", "Pressure", "pressure", "hPa");
    ha.addSensor("BME280-1", "humidity", "Humidity", "humidity", "%");
    ha.addSensor("TSL2561-1", "illuminance", "Illuminance", "lx");
    ha.addSensor("TSL2561-1", "unitilluminance", "Unit-Illuminance", "", "[0..1]", "mdi:sun-wireless");
    ha.addSensor("RAIN-1", "unitrain", "Unit-rain", "", "[0..1]", "mdi:weather-pouring");
    ha.addBinarySensor("RAIN-1", "rain", "Rain", "", "", "mdi:weather-pouring");
    ha.addBinarySensor("LIGHTNING-WARNING", "state", "Lightning Warning", "", "", "mdi:flash-alert");
    ha.addBinarySensor("LIGHTNING-NORMAL", "state", "Lightning no activity", "", "", "mdi:flash-alert");
    ha.addBinarySensor("LIGHTNING-EVENT", "state", "Lightning Active!", "", "", "mdi:flash-alert");
    ha.addSensor("LIGHTNING-EVENT", "counter", "Lightning count", "", "N", "mdi:flash-alert");
#endif
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
