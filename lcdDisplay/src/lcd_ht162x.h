// display_digits_ht1621.h
/* 
   References:
 * Datasheet: https://www.holtek.com/productdetail/-/vg/ht1621

Research from following projects has been useful:
* anxzhu (github.com/anxzhu) https://github.com/anxzhu/segment-lcd-with-ht1621
* Valerio Nappi (github.com/5N44P) https://github.com/5N44P/ht1621-7-seg
* https://github.com/MartyMacGyver/LCD_HT1622_16SegLcd
*/

#pragma once

#include "muwerk.h"
#include "SevenSegmentASCII.h"

namespace ustd {

class DisplayDigitsHT162x {
  public:
    const char *version = "0.1.0";
    enum LcdType {lcd12digit_7segment, lcd10digit_16segment};

    // See command summary, datasheet p.13:
    //                            cmd prefix   command      comment
    //                                   ---   -----------  ------------
    const uint8_t cmdBIAS    = 0x52;  // 100 | 0 010a bXcX, ab=10 4 commons option, c=1 1/3 cmdcmdBIAS option 
    const uint8_t cmdSYS_DIS = 0X00;  // 100 | 0 0000 000X, Turn off both system oscillator and LCD bias generator
    const uint8_t cmdSYS_EN  = 0X02;  // 100 | 0 0000 001X, Turn on system oscillator
    const uint8_t cmdLCD_OFF = 0X04;  // 100 | 0 0000 010X, Turn off LCD bias generator
    const uint8_t cmdLCD_ON  = 0X06;  // 100 | 0 0000 011X, Turn on LCD bias generator
    const uint8_t cmdWDT_DIS = 0X0A;  // 100 | 0 0000 101X, Disable WDT time-out flag output
    const uint8_t cmdRC_256K = 0X30;  // 100 | 0 0011 0XXX, System clock source, on-chip RC oscillator

  private:
    // muwerk task management
    Scheduler *pSched;
    int tID;

    static const uint8_t frameBufferSize=12; // 12 positions of 8bit used for HT1621, 11 positions of 16bit used for HT1622 
    uint16_t frameBuffer[frameBufferSize];  // cache for segment state: only rewrite segments on content change

    // device configuration
    String name;
    LcdType lcdType;
    uint8_t csPin;
    uint8_t wrPin;
    uint8_t dataPin;
    uint8_t lcdBacklightPin;
    uint8_t pwmIndexEsp32;

    int digitCnt;
    int digitRawCnt;
    int segmentCnt;

    bool isActive = false;
    bool isOn = false;
    uint8_t backlight = 0x0;

    long clearDelayMs = 2;
    long printDelayMs = 2;
    long writeDelayUs = 4;

  public:

    DisplayDigitsHT162x(String name, LcdType lcdType, uint8_t csPin, uint8_t wrPin, uint8_t dataPin, uint8_t lcdBacklightPin=-1, uint8_t pwmIndexEsp32=-1)
        : name(name), lcdType(lcdType), csPin(csPin), wrPin(wrPin), dataPin(dataPin), lcdBacklightPin(lcdBacklightPin), pwmIndexEsp32(pwmIndexEsp32) {
    /*! Instantiates a DisplayDigitsHT162x mupplet
     *
     * No hardware interaction is performed, until \ref begin() is called.
     *
     * @param name Name of the lcd display, used to reference it by pub/sub messages
     * @param lcdType \ref LcdType 
     */
        switch (lcdType) {
            case LcdType::lcd12digit_7segment:
                digitCnt=12; 
                digitRawCnt=13;  // Chinese phone-booth title snips at display top
                segmentCnt=8;
                isActive=true;
                break;
            case LcdType::lcd10digit_16segment:
                digitCnt=10; 
                digitRawCnt=12;  // Special case last digits encodes 9 decimal dots  
                segmentCnt=17;
                isActive=true;
                break;
            default:
                isActive=false;
        }
    }

    /*! Initialize the display hardware and start operation
     * @param _pSched       Pointer to a muwerk scheduler object, used to create worker
     *                      tasks and for message pub/sub.
     * @return true if display is active.
     */
    bool begin(Scheduler *_pSched) {
        pSched = _pSched;
        //tID = pSched->add([this]() { this->loop(); }, name, 10000L);

        if (isActive) {
            pinMode(csPin, OUTPUT);
            pinMode(wrPin, OUTPUT);
            pinMode(dataPin, OUTPUT);
            if (lcdBacklightPin!=-1) pinMode(lcdBacklightPin, OUTPUT);

            switch (lcdType) {
                case LcdType::lcd12digit_7segment:
                    writeCmd(cmdBIAS);     // set LCD bias
                    writeCmd(cmdRC_256K);  // use internal clock
                    writeCmd(cmdSYS_DIS);  // disable all generators
                    writeCmd(cmdWDT_DIS);  // disable watchdog timer output bitj
                    writeCmd(cmdSYS_EN);   // enable generators
                    setDisplay(true);       // switch on display
                    clear();                // Clear display
                    break;
                case LcdType::lcd10digit_16segment:
                    writeCmd(cmdBIAS);     // set LCD bias
                    writeCmd(cmdRC_256K);  // use internal clock
                    writeCmd(cmdSYS_DIS);  // disable all generators
                    writeCmd(cmdWDT_DIS);  // disable watchdog timer output bitj
                    writeCmd(cmdSYS_EN);   // enable generators
                    setDisplay(true);       // switch on display
                    clear();                // Clear display
                    break;
                default:
                    isActive=false;
                    break;
            }
        }
        return isActive;
    }

    void setDisplay(bool state) {
        /*! Switch display on or off
        @param state true: display on, else off.
        */
        if (state) {
            writeCmd(cmdLCD_ON);
            isOn=true;
        } else {
            writeCmd(cmdLCD_OFF);
            isOn=false;
        }
    }

    void setBacklight(bool state)
    /*! Set backlight state (if available for display-type)
    @param state backlight state
    */
    {
        if (lcdBacklightPin!=-1) {
            if (state) {
                digitalWrite(lcdBacklightPin, HIGH);
                backlight=0xff;
            } else {
                digitalWrite(lcdBacklightPin, LOW);
                backlight=0x0;
            }
            delay(1);
        }
    }

    void setBacklight(uint16_t level)
    /*! Set backlight pwm level (if available for display-type)
    @param state backlight pwm level XXX unitluminance.
    */
    {
        if (lcdBacklightPin!=-1) {
            analogWrite(lcdBacklightPin, level);
            delay(1);
        }
    }
    
    void clear() {
        /*! Clear display */
        writeClear();
    }

    void printAt(uint8_t pos, char c, bool decimalDot=false) {
        /*! Print a character at position on display

        This function encodes a character
        Use /ref setSegmentPos() to set individual segments of a digit

        @param pos Position to print, zero-based, starting from left, must be smaller than number of digits of display
        @param c Character to be printed. Needs to be in space of printable characters (e.g. [0-9,A-Z])
        @param decimalDot On true, decimal dot is switched on
        */
        uint8_t segs = encodeDigit(c);
        if (pos<digitCnt) {
            writeDigit(pos, (uint8_t)segs, decimalDot);
        } 
    }

    void print(String s, bool alignRight=true) {
        /*! Print a string to display

        If a string is longer than display size, the string is clipped. If the string is
        shorter, it's padded with spaces, either on the left (alignRight=true) or on the right.

        The print command always draws on the entire display, however only segments that have
        actually changed are physically rewritten (frame buffering).

        @param s String to be printed
        @param alignRight Boolean to either select right- (default) or left-alignement.
        */
        int i,l;
        int ind;
        bool dot=false;

        String pm;

        l=s.length();
        if (alignRight) {
            ind=digitCnt-1;
            for (i=l-1; i>=0; i--) {
                if (ind<0) break;
                if (s[i]=='.') {
                    if (dot) {
                        printAt(ind,'.',true);
                        --ind;
                    } else {
                        dot=true;
                    }
                } else {
                    Serial.print(ind);
                    if (dot)
                    pm=String(": ")+String(s[i])+String(".");
                    else
                    pm=String(": ")+String(s[i]);

                    Serial.println(pm);
                    printAt(ind, s[i], dot);
                    dot=false;
                    --ind;
                }
            }
            if (dot && ind>=0) {
                Serial.print(ind);
                pm=String(": .");
                Serial.println(pm);
                printAt(ind, '.', true);
                dot=false;
                --ind;
            }
            if (i>=0) for (i=0; i<ind; i++) {
                Serial.print(i);
                Serial.println(": _");
                printAt(i,' ', false);
            }
        } else {
            ind=0;
            for (i=0; i<l; i++) {
                if (ind>=digitCnt) break;
                if (i<l-1) {
                    if (s[i]!='.' && s[i+1]=='.') {
                        printAt(ind,s[i],true);
                        ++ind;
                        ++i;
                        continue;
                    }
                } 
                printAt(ind, s[i], dot);
                ++ind;
            }
            for (i=ind; i<digitCnt; i++) printAt(i,' ', false);
        }
        delay(printDelayMs);
    }

    void setSegmentsAt(uint8_t pos, uint8_t segments) {
        /*! Set individual segments of a display character
        @param pos digit position, zero-based, counted from left
        @param segments bits of segments to be set.
        */
        if (pos<digitRawCnt) {
            writeDigit(pos, segments, false);
        } 
    }

    void setSegmentsAt(uint8_t pos, uint16_t segments) {
        /*! Set individual segments of a display character
        @param pos digit position, zero-based, counted from left
        @param segments bits of segments to be set.
        */
        if (pos<digitRawCnt) {
            writeDigit(pos, segments, false);
        }
    }


  private:
    void writeDigit(uint8_t addr, uint8_t segmentData, bool decimalDot)
    {
    /* Correspondance data-bits to 7(+1 decimal dot) segments on display:
              |-3-|
              7   2
              |-6-|
              5   1
              |-4-|0
    */
        switch (lcdType) {
            case LcdType::lcd12digit_7segment:
                if (decimalDot) {
                    segmentData |= 1;
                } else {
                    segmentData &= 0xfe;
                }
                break;
            default:
                break;
        }
        //uint8_t adsh = (addr*2) << 2;
        uint8_t adsh = addr << 3;
        if (addr < frameBufferSize) {
            if (frameBuffer[addr]==segmentData) return;
            else frameBuffer[addr]=segmentData;
        }
        digitalWrite(csPin, LOW);
        writeData((uint8_t)0xa0, 3);  // 3 bit READ command 0b101
        writeData((uint8_t)adsh, 6);  // 6 bit address
        writeData(segmentData, 8); // successive write 2x4 bit
        digitalWrite(csPin, HIGH);
    }

    void writeDigit(uint8_t addr, uint16_t segmentData, bool decimalDot)
    {
        switch (lcdType) {
            case LcdType::lcd10digit_16segment:
                break;
            default:
                break;
        }
        //uint8_t adsh = (addr*2) << 3;
        uint8_t adsh = addr << 4;
        if (addr < frameBufferSize) {
            if (frameBuffer[addr]==segmentData) return;
            else frameBuffer[addr]=segmentData;
        }
        digitalWrite(csPin, LOW);
        writeData((uint8_t)0xa0, 3);  // 3 bit READ command 0b101
        writeData((uint8_t)adsh, 6);  // 6 bit address
        writeData(segmentData, 16); // successive write 4x4 bit
        digitalWrite(csPin, HIGH);
    }

    void writeData(uint8_t data, uint8_t cnt) {
        /*! write the cnt upper bits of data */
        // This disables interrupts for about 70uS
        uint8_t i;
        noInterrupts();
        for (i = 0; i < cnt; i++) {
            digitalWrite(wrPin, LOW);
            delayMicroseconds(writeDelayUs);
            if (data & 0x80) {
                digitalWrite(dataPin, HIGH);
            } else {
                digitalWrite(dataPin, LOW);
            }
            digitalWrite(wrPin, HIGH);
            delayMicroseconds(writeDelayUs);
            data = data << 1;
        }
        interrupts();
    }

    void writeData(uint16_t data, uint8_t cnt) {
        /*! write the cnt upper bits of data */
        // This disables interrupts for about 150uS
        uint8_t i;
        noInterrupts();
        for (i = 0; i < cnt; i++) {
            digitalWrite(wrPin, LOW);
            delayMicroseconds(writeDelayUs);
            if (data & 0x8000) {
                digitalWrite(dataPin, HIGH);
            } else {
                digitalWrite(dataPin, LOW);
            }
            digitalWrite(wrPin, HIGH);
            delayMicroseconds(writeDelayUs);
            data = data << 1;
        }
        interrupts();
    }

    void writeCmd(uint8_t cmd) {
        digitalWrite(csPin, LOW);
        writeData((uint8_t)0x80, 4); // write 3 bits command mode 0b100, plus one zero bit 9 for command (unused)
        writeData(cmd, 8);  // write 8 command bits
        digitalWrite(csPin, HIGH);
    }

    void writeClear() {
        uint8_t addr = 0;
        uint8_t adsh;
        uint8_t i;
        for (i = 0; i < digitRawCnt; i++) {
            switch (lcdType) {
                case LcdType::lcd12digit_7segment:
                    adsh = (addr*2) << 2;
                    digitalWrite(csPin, LOW);
                    writeData((uint8_t)0xa0, 3);
                    writeData(adsh, 6);
                    writeData((uint8_t)0x0, 8);
                    digitalWrite(csPin, HIGH);
                    addr = addr + 1;
                    break;
                case LcdType::lcd10digit_16segment:
                    adsh = (addr*2) << 3;
                    digitalWrite(csPin, LOW);
                    writeData((uint8_t)0xa0, 3);
                    writeData(adsh, 6);
                    writeData((uint16_t)0x0, 16);
                    digitalWrite(csPin, HIGH);
                    addr = addr + 1;
                    break;
            }
        }
/*
        if (lcdType==LcdType::lcd10digit_16segment) { // 9 pesky decimal dots
            for (int ad=0x29; ad<0x2b; ad++) {
                adsh = ad << 2;
                digitalWrite(csPin, LOW);
                writeData((uint8_t)0xa0, 3);
                writeData(adsh, 6);
    //            writeData((uint16_t)0xffff, 16);
                writeData((uint16_t)0x77, 16);
                delay(1000);
                digitalWrite(csPin, HIGH);

            }
            for (int ad=0x29; ad<0x2b; ad++) {
                adsh = ad << 2;
                digitalWrite(csPin, LOW);
                writeData((uint8_t)0xa0, 3);
                writeData(adsh, 6);
    //            writeData((uint16_t)0xffff, 16);
                writeData((uint16_t)0x7000, 16);
                delay(1000);
                digitalWrite(csPin, HIGH);

            }
            for (int ad=0x29; ad<0x2b; ad++) {
                adsh = ad << 2;
                digitalWrite(csPin, LOW);
                writeData((uint8_t)0xa0, 3);
                writeData(adsh, 6);
    //            writeData((uint16_t)0xffff, 16);
                writeData((uint16_t)0x00, 16);
                digitalWrite(csPin, HIGH);
                
            }
        }
*/

        for (i=0; i<frameBufferSize; i++) {
            frameBuffer[i]=0;
        }
        delay(clearDelayMs);
    }


    /* Correspondance data-bits to 7(+1 decimal dot) segments on display and font:
             Display       Font (bits)
              |-3-|       |-0-|
              7   2       5   1
              |-6-|       |-6-|
              5   1       4   2
              |-4-|0      |-3-|7
    */
   uint8_t encodeDigit(char c) {
       if (c>=32 && c<128) {
           uint8_t segs=0,fsegs;
            fsegs=SevenSegmentASCII[c-32];
            if (fsegs & (1 << 0)) segs |= (1 << 3);
            if (fsegs & (1 << 1)) segs |= (1 << 2);
            if (fsegs & (1 << 2)) segs |= (1 << 1);
            if (fsegs & (1 << 3)) segs |= (1 << 4);
            if (fsegs & (1 << 4)) segs |= (1 << 5);
            if (fsegs & (1 << 5)) segs |= (1 << 7);
            if (fsegs & (1 << 6)) segs |= (1 << 6);
            if (fsegs & (1 << 7)) segs |= (1 << 0);
            return segs;
       }
       return 0;
   }
    /*
    uint8_t digits7Segment[10]={0b10111110, 0b00000110, 0b01111100, 0b01011110, 0b11000110, 
                      0b11011010, 0b11111010, 0b00001110, 0b11111110, 0b11011110};
    uint8_t letters7Segment[26]={
        // A
        0b11101110, 0b11110010, 0b01110000, 0b01110110, 0b11111000, 0b11101000,
        // G
        0b10111010, 0b11100110, 0b00000010, 0b00110110, 0b11100101, 0b10110000,
        // M
        0b10101110, 0b01100010, 0b01110010, 0b11101100, 0b11001111, 0b01100000,
        // S
        0b11011010, 0b11110000, 0b10110110, 0b00110010, 0b10110111, 0b11100110,
        //Y
        0b11100100, 0b01111100 }; 
    uint8_t encodeDigit(char c) {

        if (c>='0' && c<='9') {
            return digits7Segment[c-'0'];
        }
        if (c>='a' && c<='z') c -= 'a'-'A';
        if (c>='A' && c<='Z') {
            return letters7Segment[c-'A'];
        }
        return 0;
    }
    */


    void loop() {
    }
};


}  // namespace ustd