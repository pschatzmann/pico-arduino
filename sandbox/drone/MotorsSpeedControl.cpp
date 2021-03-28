#include "MotorsSpeedControl.h"
#include "PicoPWM.h"
#include "PicoLogger.h"

/// Motor Speed Control implementation for the Raspberry Pico using my PicoPWM class

#ifdef PICO

PicoPWM pwm(50000, 1860);

void MotorsSpeedControl::Init() {
    // Set motors pin PD4, PD5, PD6, PD7 as outputs
    for (int id=4; id<=7;id++){
        pwm.begin(id);
    }
}

void MotorsSpeedControl::Idle() {
    for (int id = 4; id <= 7; id++){
        pwm.write(id, MIN_POWER);
    }
}

void MotorsSpeedControl::UpdateSpeed(int _id, float _PWM) {
    Logger.printf(PicoLogger::Debug, "UpdateSpeed: %d, %f \n", _id, _PWM);
    pwm.write(_id, _PWM);
}

void MotorsSpeedControl::ApplySpeed(volatile uint16_t *TCNTn, volatile uint16_t *OCRnA) {
    // nothing to do - update was directly done in UpdateSpeed
}
#endif
