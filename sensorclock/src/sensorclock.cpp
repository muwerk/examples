// #define USE_SERIAL_DBG 1

#include "ustd_platform.h"
#include "scheduler.h"

#include "console.h"

#include "net.h"
#include "mqtt.h"
#include "ota.h"
#include "web.h"

#include "mup_clock_7seg.h"
#include "mup_illuminance_tsl2561.h"
#include "mup_temphum_dht.h"
#include "mup_presstemp_bmp180.h"
#include "mup_switch.h"
#include "mup_co2_ccs811.h"
#include "i2cdoctor.h"

#include "mup_gfx_panel.h"
#include "home_assistant.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;

ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
ustd::Web web;
ustd::HomeAssistant ha("Sensorclock", "MuWerk Intl.", "SC-Revisited", "1.1.0");

ustd::I2CDoctor doctor("doctor");
ustd::IlluminanceTSL2561 illumin("TSL2561-1", ustd::IlluminanceTSL2561::FilterMode::LONGTERM, ustd::IlluminanceTSL2561::IntegrationMode::LONGTERM402ms, ustd::IlluminanceTSL2561::GainMode::HIGH16x);
ustd::TempHumDHT dht("DHT22-1", 0, 0, ustd::TempHumDHT::DHTType::DHT22);  // port 0 == D3
ustd::PressTempBMP180 pressure("BMP085-1");
#ifdef I2C_D1_D2
ustd::Clock7Seg clock7("clock", 0x70, 14, true, "TSL2561-1/sensor/unitilluminance");
ustd::Switch sw1("sensorclock1", 13);
ustd::Switch sw2("sensorclock2", 12, ustd::Switch::Mode::Default, false, "clock/alarm/off");
ustd::GfxPanel display("display", ustd::GfxDrivers::DisplayType::SSD1306, 128, 64, 0x3c);
#else
ustd::Clock7Seg clock7("clock", 0x70, 14, true, "tsl2561/sensor/unitilluminance");
#endif
ustd::CO2CCS811 airq("CCS811-1", ustd::CO2CCS811::FilterMode::MEDIUM, 0x5a, "DHT22-1/sensor/temperature", "DHT22-1/sensor/humidity");

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Startup");
#endif  // USE_SERIAL_DBG
    doctor.begin(&sched, &Wire);
    con.begin(&sched);

    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    web.begin(&sched);

#if defined(I2C_D1_D2)
#ifdef USE_SERIAL_DBG
    Serial.println("Using slightly non-standard I2C port D1, D2");
#endif
    Wire.begin(D1, D2);  // SDA, SCL; Non-standard, from the old days...
    display.begin(&sched, &mqtt);
#endif

    clock7.begin(&sched);
    clock7.maxAlarmDuration = 10;
    // illumin.setMaxLux(200);
    illumin.begin(&sched, &Wire, 10000);
    dht.begin(&sched);
    pressure.begin(&sched);
    pressure.setReferenceAltitude(518.0);  // 518m above NN, now we also receive PressureNN values for sea level.
#ifdef I2C_D1_D2
    sw1.begin(&sched);
    sw2.begin(&sched);
#endif
    airq.begin(&sched);

    ha.begin(&sched, true);

    ha.addSensor("BMP085-1", "temperature", "Temperature", "temperature", "°C");
    ha.addSensor("BMP085-1", "pressureNN", "Pressure NN", "pressure", "hPa");
    ha.addSensor("BMP085-1", "pressure", "Pressure", "pressure", "hPa");
    ha.addSensor("DHT22-1", "temperature", "Temperature", "temperature", "°C");
    ha.addSensor("DHT22-1", "humidity", "Humidity", "humidity", "%");
    ha.addSensor("TSL2561-1", "unitilluminance", "Unit-Illuminance", "", "[0..1]", "mdi:sun-wireless");
    ha.addSensor("TSL2561-1", "illuminance", "Illuminance", "illuminance", "lx");
    ha.addSensor("CCS811-1", "co2", "CO2", "carbon_dioxide", "ppm");
    ha.addSensor("CCS811-1", "voc", "VOC", "volatile_organic_compounds", "ppb");
    /*int tID =*/sched.add(appLoop, "main",
                           1000000);  // every 1000000 micro sec = once a second call appLoop

    // Use Home Assistant's auto-discovery to register sensors:
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
