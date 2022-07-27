#include "ustd_platform.h"
#include "scheduler.h"
#include "net.h"
#include "mqtt.h"
#include "ota.h"
#include "jsonfile.h"

#include "console.h"
#include "mup_oled.h"

void appLoop();

ustd::Scheduler sched(10, 16, 32);
ustd::SerialConsole con;
ustd::Net net(LED_BUILTIN);
ustd::Mqtt mqtt;
ustd::Ota ota;
ustd::jsonfile jf;

ustd::SensorDisplay display(128,64,0x3c);

uint8_t current_slots;

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Starting up...");
#endif  // USE_SERIAL_DBG
    ustd::array<String> topics; 
    ustd::array<String> captions;
    ustd::array<String> msgs;
    String layout, formats;
    ustd::array<double> dirs={0,0,0,0};
    for (uint8_t i=0; i<current_slots; i++) {
        msgs[i]="init";
        captions[i]="room";
        topics[i]="some/topic";
    }
    layout=jf.readString("display/layout","SS|SS");
    if (layout!="SS|SS" && layout!="L|SS" && layout!="L|L" && layout !="SS|L" && layout != "L" && layout != "SS") {
        layout="SS|SS";
#ifdef USE_SERIAL_DBG
        Serial.print("Unsupported layout: ");
        Serial.print(layout);
        Serial.println(" please use (64x128 displays) 'SS|SS' or 'L|SS' or 'L|L' or 'SS|L' or (32x128 displays) 'L' or 'SS' ");
#endif  // USE_SERIAL_DBG
    }
    formats=jf.readString("display/formats", "FFFF"); // four floats, either F (float), messages are converted to float fir %.1f, or S (string) messages displayed as-is.
    while (formats.length()<4) {
        formats += " ";
    }
    for (unsigned int i=0; i<formats.length(); i++) {
        if (formats[i]!='F' && formats[i]!='S' && formats[i]!=' ') {
            formats[i]='S';
#ifdef USE_SERIAL_DBG
            Serial.print("Unsupported formats string: ");
            Serial.print(formats);
            Serial.println(" should only contain 'F' for float, 'S' for string, or ' '.");
#endif  // USE_SERIAL_DBG
        }
    }

    jf.readStringArray("display/topics", topics);
    jf.readStringArray("display/captions", captions);
    current_slots=topics.length();
    display.begin(current_slots, layout, formats, topics, captions);
    display.updateDisplay(msgs,dirs);

    con.begin(&sched);
    net.begin(&sched);
    mqtt.begin(&sched);
    ota.begin(&sched);
    int tID = sched.add(appLoop, "main", 1000000);

    auto fnall = [=](String topic, String msg, String originator) {
            display.sensorUpdates(topic, msg, originator);
        };
    for (uint8_t i=0; i<current_slots; i++) {
#ifdef USE_SERIAL_DBG
        Serial.print("Subscribing: ");
        Serial.println(topics[i]);
#endif
        mqtt.addSubscription(tID, topics[i], fnall);
    }
    // sched.subscribe(tID, "DHT-1/sensor/temperature", fnall);
}

void appLoop() {
    const char *weekDays[]={"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    const char *pDay;
    struct tm *plt;
    time_t t;
    char buf[64];
    static char oldbuf[64]="";
    // scruffy old c time functions 
    t=time(nullptr);
    plt=localtime(&t);
    pDay=weekDays[plt->tm_wday];
    // sprintf(buf,"%s %02d. %02d:%02d",pDay,plt->tm_mday, plt->tm_hour, plt->tm_min);
    sprintf(buf,"%s %02d:%02d",pDay, plt->tm_hour, plt->tm_min);
    if (strcmp(buf,oldbuf)) {
        strcpy(oldbuf,buf);
        sched.publish("clock/timeinfo", String(buf));
    }
    // If a sensors doesn't update values for 1 hr (3600sec), declare invalid.
    for (uint8_t i=0; i<current_slots; i++) {
        if (time(nullptr)-display.lastUpdates[i] > 3600) {
            display.vals_init[i]=false;
        }
    }
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}