#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"
#include "jsonfile.h"

#include "console.h"
#include "mup_gfx_panel.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
#ifdef __ESP32S2__
ustd::Net net(2); // ESP32S2 forgot about LED_BUILTIN...
#else
ustd::Net net(LED_BUILTIN);
#endif
ustd::Mqtt mqtt;
ustd::Ota ota;

// make sure that two json files exist: display1.json and display2.json, corresponding to the names of the GfxPanel s.
#ifdef __ESP32__
#ifdef __ESP32S2__
//                                                                                     CS  DC  RST
ustd::GfxPanel displayTft("display1", ustd::GfxDrivers::DisplayType::ST7735, 160, 128,  5, 16, 17, "DE");
#else
//                                                                                     CS  DC  RST
ustd::GfxPanel displayTft("display1", ustd::GfxDrivers::DisplayType::ST7735, 160, 128,  5, 16, 17, "DE");
#endif // __ESP32S2__
#else
//                                                                                     CS  DC  RST
//ustd::GfxPanel displayTft("display1", ustd::GfxDrivers::DisplayType::ST7735, 128, 128, D4, D3, (uint8_t)-1, "DE");
ustd::GfxPanel displayOled("display2", ustd::GfxDrivers::DisplayType::SSD1306, 128,64, 0x3c, &Wire, "DE");
#endif

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Starting up...");
#endif  // USE_SERIAL_DBG

    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);

#ifdef __ESP32__
Serial.println("Starting display");
    displayTft.begin(&sched,&mqtt);
Serial.println("Display started");
#else
    const char *topics[]={"clock/timeinfo", "!hastates/sensor/klima_nordseite_temperature/state", "!hastates/sensor/klima_kuche_temperature/state"};
    const char *captions[]={"Time", "Nord _C", "Kueche _C"};
    displayOled.begin(&sched,&mqtt,"S|ff",3,topics,captions);
#endif
    sched.add(appLoop, "main", 1000000);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
