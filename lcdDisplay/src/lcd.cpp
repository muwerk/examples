#define USE_SERIAL_DBG 1

#include "scheduler.h"
#include "lcd_ht162x.h"

ustd::DisplayDigitsHT162x numeric("numeric", ustd::DisplayDigitsHT162x::LcdType::lcd12digit_7segment, 4,5,6);
//ustd::DisplayDigitsHT162x alphanum("alpha", ustd::DisplayDigitsHT162x::LcdType::lcd10digit_16segment, 9,10,11,3);

void appLoop();

ustd::Scheduler sched(10, 16, 32);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Startup");
#endif  // USE_SERIAL_DBG
    /*int tID =*/ sched.add(appLoop, "main", 500000);
    numeric.begin(&sched);
    //alphanum.begin(&sched);
    
    //alphanum.setBacklight((uint16_t)32);

    //alphanum.clear();
    numeric.clear();
    delay(10);

    Serial.print("Free memory: ");
    Serial.println(freeMemory());
    
    String msg;
    bool right=true;
    for (int j=0; j<4; j++) {
        msg="[";
        for (int i=0; i<10; i++) {
            msg+=String(i%10)+".";
            String pm=msg+"]";
            numeric.print(pm,right);
            delay(500);
        }
        numeric.clear();
        right=!right;
    }
  

    /*
    for (int i=0; i<10; i++)
        digits.setSegmentsAt(i,(uint16_t)0x0);
    delay(2000);
    for (int i=0; i<10; i++) {
        uint16_t x=0;
        for (int j=0; j<16; j++) {
            x |= 0x1 << j;
            digits.setSegmentsAt(i,(uint16_t)x);
            delay(10);
        }
        digits.setSegmentsAt(i,(uint16_t)0xffff);
    }
    delay(2000);
    for (int i=0; i<10; i++)
        digits.setSegmentsAt(i,(uint16_t)0x0);
    */
    //digits.setSegmentsAt(2,(uint16_t)0xffff);

    // digits.print("Hello world");
    // delay(2000);
    //digits.clear();
    //delay(2000);
    //digits.print("3.1415");
    //delay(1000);
    //digits.print("3.141592");
    //delay(1000);
    //digits.clear();
    /*
    digits.print("ABCDEFGHIJKL");
    delay(2000);
    digits.print("MNOPQRSTUVWX");
    delay(2000);
    digits.print("YZ0123456789");
    delay(2000);
    */
}

int n=0;
bool bl=false;
void appLoop() {
    bl ? bl=false : bl=true;
    digitalWrite(LED_BUILTIN, bl);
/*
    String msg=String(n)+" Hz";
    //while (msg.length()<12) msg=' '+msg;
    numeric.print(msg);
    ++n;
*/
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}
