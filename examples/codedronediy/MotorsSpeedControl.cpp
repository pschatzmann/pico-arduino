#include "MotorsSpeedControl.h"
#include "PicoPWM.h"
#include "PicoLogger.h"

/// Motor Speed Control implementation for the Raspberry Pico using my PicoPWM class

#ifdef PICO

#ifndef MOTOR_START_PIN
#define MOTOR_START_PIN 4
#endif

#ifndef MOTOR_PWM_FREQUENCY
#define MOTOR_PWM_FREQUENCY 50000
#endif

int motor_pins[4];
int pwm_frequency = MOTOR_PWM_FREQUENCY;
PicoPWM *pwm;

void MotorsSpeedControl::Init() {
    // setup PicoPWM
    if (pwm==nullptr) {
        pwm = new PicoPWM(pwm_frequency, MAX_POWER);
    }
    // Set motors pin PD4, PD5, PD6, PD7 as outputs
    for (int id=0; id<4;id++){
        motor_pins[id] = MOTOR_START_PIN + id;
        pwm->begin(motor_pins[id]);
    }
}

void MotorsSpeedControl::Idle() {
    for (int id=0; id<4;id++){
        pwm->write(motor_pins[id], MIN_POWER);
    }
}

void MotorsSpeedControl::UpdateSpeed(int id, float pulse_width) {
    if (pulse_width>MAX_POWER){
        pulse_width = MAX_POWER;
    } else if (pulse_width<MIN_POWER){
        pulse_width = MIN_POWER;
    }
    Logger.printf(PicoLogger::Debug, "UpdateSpeed: %d, %f \n", id, pulse_width);
    pwm->write(motor_pins[id], pulse_width);
}

void MotorsSpeedControl::ApplySpeed(volatile uint16_t *TCNTn, volatile uint16_t *OCRnA) {
    // nothing to do - update was directly done in UpdateSpeed
}
#endif
