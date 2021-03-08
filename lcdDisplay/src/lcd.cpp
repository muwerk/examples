#define USE_SERIAL_DBG 1

#include "scheduler.h"
#include "display_digits_hd1621.h"

ustd::DisplayDigitsHD1621 digits("screen", 9,10,11,-1,12,7);

void appLoop();

ustd::Scheduler sched(10, 16, 32);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Startup");
#endif  // USE_SERIAL_DBG
    int tID = sched.add(appLoop, "main", 10000);
    digits.begin(&sched, true);

    digits.printSegmentsAt(12,0xff);
    digits.print("ABCDEFGHIJKL");
    delay(2000);
    digits.print("MNOPQRSTUVWX");
    delay(2000);
    digits.print("YZ0123456789");
    delay(2000);
}

int n=0;
void appLoop() {
    String msg=String(n)+" Hz";
    while (msg.length()<12) msg=' '+msg;
    digits.print(msg);
    ++n;
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
