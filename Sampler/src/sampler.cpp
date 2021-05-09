#include "ustd_platform.h"
#include "scheduler.h"

#include "../../../Research-mupplets/mp3.h"

#include "mup_light.h"
#include "mupplet_core.h"

void appLoop();

#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_8K
ustd::Scheduler sched(2, 2, 2);
#else
ustd::Scheduler sched(10, 16, 32);
#endif

ustd::Light led1("myLed1", 11, true);
ustd::Light led2("myLed2", 12, true);

//                      RX    TX
// HardwareSerial Serial1(10, 9);
ustd::Mp3Player *pPlayer;

// HardwareSerial Serial1(PA10, PA9);
// HardwareSerial Serial3(PA10, PA9);
void setup() {
    led1.begin(&sched);
    led2.begin(&sched);
    Serial.setRx(PA10);  // using pin name PY_n
    Serial.setTx(PA9);   // using pin number PYn
    pPlayer = new ustd::Mp3Player("mp3", &Serial, ustd::Mp3Player::MP3_PLAYER_TYPE::DFROBOT);
    pPlayer->begin(&sched);
    led1.setMode(ustd::LightController::Mode::Wave, 2000, 0.0);
    led2.setMode(ustd::LightController::Mode::Wave, 2000, 0.5);
    delay(100);
    pPlayer->setVolume(10);
    delay(100);
    pPlayer->playFolderTrack(1, 1);
    sched.add(appLoop, "1", 1000000L);
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}