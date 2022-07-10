#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"

#include "console.h"
#include "mup_presstemp_bmp180.h"

#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
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

ustd::PressTempBMP180 bmp("myBMP180", ustd::PressTempBMP180::FilterMode::FAST);

void updateDisplay(String msg1, String msg2, double d1, double d2) {
    display.clearDisplay();

    display.setCursor(10,28);
    display.println(msg1);
    display.setCursor(10,63);
    display.println(msg2);

    if (d1!=0.0) {
        display.drawLine(5, 12, 5, 4, SSD1306_WHITE);
        display.drawLine(6, 12, 6, 4, SSD1306_WHITE);
        if (d1>0.0) { // up arrow
            display.drawLine(5, 4, 2, 7, SSD1306_WHITE);
            display.drawLine(5, 4, 8, 7, SSD1306_WHITE);
            display.drawLine(6, 4, 3, 7, SSD1306_WHITE);
            display.drawLine(6, 4, 9, 7, SSD1306_WHITE);
        } else { // down arrow
            display.drawLine(5, 12, 2, 9, SSD1306_WHITE);
            display.drawLine(5, 12, 8, 9, SSD1306_WHITE);
            display.drawLine(6, 12, 3, 9, SSD1306_WHITE);
            display.drawLine(6, 12, 9, 9, SSD1306_WHITE);
        }
    }
    if (d2!=0.0) {
        display.drawLine(5, 36, 5, 44, SSD1306_WHITE);
        display.drawLine(6, 36, 6, 44, SSD1306_WHITE);
        if (d2>0.0) { // up arrow
            display.drawLine(5, 36, 2, 39, SSD1306_WHITE);
            display.drawLine(5, 36, 8, 39, SSD1306_WHITE);
            display.drawLine(6, 36, 3, 39, SSD1306_WHITE);
            display.drawLine(6, 36, 9, 39, SSD1306_WHITE);
        } else { // down arrow
            display.drawLine(5, 44, 2, 41, SSD1306_WHITE);
            display.drawLine(5, 44, 8, 41, SSD1306_WHITE);
            display.drawLine(6, 44, 3, 41, SSD1306_WHITE);
            display.drawLine(6, 44, 9, 41, SSD1306_WHITE);
        }
    }
    display.display();

}

double t1,t2, d1, d2;
int t1_val=0;
int t2_val=0;
time_t lastUpdate_t1=0;
time_t lastUpdate_t2=0;

#define HIST_CNT 4
double hist_t1[HIST_CNT];
double hist_t2[HIST_CNT];

// This is called on Sensor-update events
void sensorUpdates(String topic, String msg, String originator) {
    int disp_update=0;
    char buf1[64],buf2[64];
    if (topic == "myBMP180/sensor/temperature") {
        t1 = msg.toFloat();
        lastUpdate_t1=time(nullptr);
        if (!t1_val) {
            for (int i=0; i<HIST_CNT; i++) hist_t1[i]=t1;
        } else {
            for (int i=0; i<HIST_CNT-1; i++) hist_t1[i]=hist_t1[i+1];
            hist_t1[HIST_CNT-1]=t1;
        }
        d1=t1-hist_t1[0];
        t1_val=1;
        disp_update=1;
    }
    if (topic == "myBMP180/sensor/deltaaltitude") {
        t2 = msg.toFloat();
        lastUpdate_t2=time(nullptr);
        if (!t2_val) {
            for (int i=0; i<HIST_CNT; i++) hist_t2[i]=t2;
        } else {
            for (int i=0; i<HIST_CNT-1; i++) hist_t2[i]=hist_t2[i+1];
            hist_t2[HIST_CNT-1]=t2;
        }
        d2=t2-hist_t2[0];
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
            sprintf(buf2,"%8.3f",t2);
        } else {
            sprintf(buf2,"%s", "NaN");
        }
        updateDisplay(buf1,buf2,d1,d2);
    }
}

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
#endif  // USE_SERIAL_DBG
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setFont(&FreeSans12pt7b);
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font

    updateDisplay("init","init",0.0,0.0);

    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    // sensors start measuring pressure and temperature
    bmp.setReferenceAltitude(518.0); // 518m above NN, now we also receive PressureNN values for sea level.
    bmp.startRelativeAltitude(); // Use next pressureNN measurement as altitude reference
    bmp.begin(&sched, ustd::PressTempBMP180::BMPSampleMode::ULTRA_HIGH_RESOLUTION);

    // subscribe to kernel's MQTT messages, the sensorUpdates() funktion does the event handling
    // Interal BMP180 sensor
    sched.subscribe(tID, "myBMP180/sensor/temperature", sensorUpdates);
    sched.subscribe(tID, "myBMP180/sensor/deltaaltitude", sensorUpdates);
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