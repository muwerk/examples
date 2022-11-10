#define USE_HOMEASSISTANT

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
#include "mup_co2_ccs811.h"
#include "mup_gfx_panel.h"
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
ustd::HomeAssistant ha("Home-Sensor-1", "MuWerk Intl.", "Test-Version", "0.1.0");
#endif

// starts listening to mqtt messages to <hostname>/doctor/i2cinfo/get,
// then sends i2c used addresses, errors and device-count out.
ustd::I2CDoctor doctor("doctor");

ustd::TempHumDHT dht("DHT-1", D5, 0);  // name, pin, unique interrupt-id (0..9)
ustd::PressTempBMP180 bmp("BMP180-1");
ustd::CO2CCS811 co2("CCS811-1", ustd::CO2CCS811::FilterMode::MEDIUM, 0x5A, "DHT-1/sensor/temperature", "DHT-1/sensor/humidity");
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
#ifdef USE_HOMEASSISTANT
    ha.begin(&sched, true);
#endif

    dht.begin(&sched);
    bmp.begin(&sched, &Wire);
    bmp.setReferenceAltitude(518);
    ldr.begin(&sched);
    co2.begin(&sched, &Wire);

    displayOled.begin(&sched, &mqtt);
#ifdef USE_HOMEASSISTANT
    ha.addSensor("DHT-1", "temperature", "Temperature", "temperature", "°C");
    ha.addSensor("DHT-1", "humidity", "Humidity", "humidity", "%");
    ha.addSensor("LDR-1", "unitilluminance", "Unit-Illuminance", "", "[0..1]", "mdi:sun-wireless");
    ha.addSensor("BMP180-1", "temperature", "Temperature", "temperature", "°C");
    ha.addSensor("BMP180-1", "pressureNN", "Pressure NN", "pressure", "hPa");
    ha.addSensor("BMP180-1", "pressure", "Pressure", "pressure", "hPa");
    ha.addSensor("CCS811-1", "co2", "CO2", "carbon_dioxide", "ppm");
    ha.addSensor("CCS811-1", "voc", "VOC", "volatile_organic_compounds", "ppb");
#endif
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}