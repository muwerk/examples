#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_temphum_dht.h"

#include <Adafruit_GFX.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_X 128
#define SCREEN_Y 64
Adafruit_SSD1306 display(SCREEN_X, SCREEN_Y, &Wire);

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;


#ifdef __ESP32__
ustd::TempHumDHT dht("myDht", 5, 0);    // name, pin, unique interrupt-id (0..9)
ustd::TempHumDHT dht2("myDht2", 6, 1);
#else
ustd::TempHumDHT dht("myDht", D4, 0);  // name, pin, unique interrupt-id (0..9)
ustd::TempHumDHT dht2("myDht2", D7, 1);
#endif

void updateDisplay(String msg1, String msg2) {
    display.clearDisplay();

    display.setCursor(10,28);
    display.println(msg1);
    display.setCursor(10,63);
    display.println(msg2);

    display.drawLine(5, 28, 5, 4, SSD1306_WHITE);
    display.drawLine(5, 4, 3, 6, SSD1306_WHITE);
    display.drawLine(5, 4, 7, 6, SSD1306_WHITE);
    
    display.drawLine(5, 63, 5, 36, SSD1306_WHITE);
    display.drawLine(5, 36, 3, 38, SSD1306_WHITE);
    display.drawLine(5, 36, 7, 38, SSD1306_WHITE);

    display.display();

}

double t1,t2;
int t1_val=0;
int t2_val=0;
time_t lastUpdate_t1=0;
time_t lastUpdate_t2=0;

// This is called on Sensor-update events
void sensorUpdates(String topic, String msg, String originator) {
    int disp_update=0;
    char buf1[64],buf2[64];
    if (topic == "myDht/sensor/temperature") {
        t1 = msg.toFloat();
        lastUpdate_t1=time(nullptr);
        t1_val=1;
        disp_update=1;
    }
    if (topic == "myDht2/sensor/temperature") {
        t2 = msg.toFloat();
        lastUpdate_t2=time(nullptr);
        t2_val=1;
        disp_update=1;
    }
    if (disp_update) {
        if (t1_val) {
            sprintf(buf1,"%4.1f C",t1);
        } else {
            sprintf(buf1,"%s", "NaN");
        }
        if (t2_val) {
            sprintf(buf2,"%4.1f C",t2);
        } else {
            sprintf(buf2,"%s", "NaN");
        }
        updateDisplay(buf1,buf2);
    }
}

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setFont(&FreeSans18pt7b);
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font

    updateDisplay("init","init");

    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    // sensors start measuring temperature (and humidity)
    dht.begin(&sched);
    dht2.begin(&sched);

    // subscribe to kernel's MQTT messages, the sensorUpdates() funktion does the event handling
    sched.subscribe(tID, "myDht/sensor/temperature", sensorUpdates);
    sched.subscribe(tID, "myDht2/sensor/temperature", sensorUpdates);
}

void appLoop() {
    // If a sensors doesn't update values for 1/2 hr (1800sec), declare invalid.
    if (time(nullptr)-lastUpdate_t1 > 1800) {
        t1_val=0;
    }
    if (time(nullptr)-lastUpdate_t2 > 1800) {
        t2_val=0;
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}