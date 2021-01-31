#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"

 /**
 * @brief We use PWM to generate tones.
 * 
 */
class PicoTone {
    public:
        PicoTone(){            
        }

        void tone(uint8_t pinNumber, unsigned int frequency, float duty = 1.0) {
            setupTone();
            uint sliceNum = setupTonePin(pinNumber, frequency);
            pwm_set_gpio_level(pinNumber, pwm_wrap * duty);
            pwm_set_enabled(sliceNum, true);    
        }

        void noTone(uint8_t pinNumber) {
            uint sliceNum = pwm_gpio_to_slice_num(pinNumber);
            pwm_set_enabled(sliceNum, false);    
        }

protected:
    uint16_t pwm_wrap;
    uint8_t tonePins[16];
    pwm_config tone_pwm_cfg;

    void setupTone(){
        if (tonePins[0]==0) {
            // setup pwm
            tone_pwm_cfg = pwm_get_default_config();
            // ten thousand cycles per second
            pwm_config_set_clkdiv(&tone_pwm_cfg, 255);
        }
    }

    uint setupTonePin(uint8_t pinNumber, unsigned int frequency) {
        pwm_wrap = 490196 / frequency;
        uint slice = pwm_gpio_to_slice_num(pinNumber);
        if (!isTonePinDefined(pinNumber)){
            addTonePin(pinNumber);
            pwm_init(slice, &tone_pwm_cfg, false);
            gpio_set_function(pinNumber, GPIO_FUNC_PWM);
        }
        // output a square wave, so 50% duty cycle
        pwm_set_wrap(pinNumber, pwm_wrap);
        return slice;
    }

    bool isTonePinDefined(uint8_t pinNumber){
        for (int j=0;j<16;j++){
            if (tonePins[j]==pinNumber){
                return true;
            }
            if (tonePins[j]==0){
                return false;
            }
        }
        return false;
    }

    bool addTonePin(uint8_t pinNumber){
        for (int j=0;j<16;j++){
            if (tonePins[j]==pinNumber){
                return true;
            }
            if (tonePins[j]==0){
                tonePins[j]=pinNumber;
                return false;
            }
        }
        return false;
    }

};