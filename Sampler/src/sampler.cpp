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

ustd::Light led1("myLed1", PA11, true);
ustd::Light led2("myLed2", PA15, true);

//                      RX    TX
// HardwareSerial Serial1(10, 9);
ustd::Mp3Player *pPlayer;

// HardwareSerial Serial1(PA10, PA9);
// HardwareSerial Serial3(PA10, PA9);
void setup() {
    led1.begin(&sched);
    led2.begin(&sched);
    Serial1.setRx(PA10);  // using pin name PY_n
    Serial1.setTx(PA9);   // using pin number PYn
    pPlayer = new ustd::Mp3Player("mp3", &Serial1, ustd::Mp3Player::MP3_PLAYER_TYPE::DFROBOT);
    pPlayer->begin(&sched);
    led1.setMode(ustd::LightController::Mode::Wave, 2000, 0.0);
    led2.setMode(ustd::LightController::Mode::Wave, 2000, 0.5);
    // delay(3000);
    pPlayer->setVolume(20);
    delay(100);
    pPlayer->loopIndex(1);
    sched.add(appLoop, "1", 1000000L);

    // no need to configure pin, it will be done by HardwareTimer configuration
    // pinMode(pin, OUTPUT);

    // Automatically retrieve TIM instance and channel associated to pin
    // This is used to be compatible with all STM32 series automatically.
    uint8_t pwmPin = PA8;
    TIM_TypeDef *Instance =
        (TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(pwmPin), PinMap_PWM);
    uint32_t channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(pwmPin), PinMap_PWM));

    // Instantiate HardwareTimer object. Thanks to 'new' instantiation, HardwareTimer is not
    // destructed when setup() function is finished.
    HardwareTimer *MyTim = new HardwareTimer(Instance);

    // Configure and start PWM
    // MyTim->setPWM(channel, pwmPin, 5, 10, NULL, NULL); // No callback required, we can simplify
    // the function call
    MyTim->setPWM(channel, pwmPin, 440, 50);  // 440 Hertz, 50% dutycycle
}

void appLoop() {
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}