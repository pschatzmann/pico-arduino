#pragma once

#include "pico/stdlib.h"
#include <inttypes.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "PicoPinFunction.h"

/**
 * @brief The Raspberry Pico has 8 controllable PWM slices with each 2 channels, wo we can control up to 16 pwm devices at the same time!
 * This is Simple Basic PWM API where we specifiy the periods and duty cyle in Nanoseconds. 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#ifndef PWM_MAX_NUMER
#define PWM_MAX_NUMER 65535
#endif

class PicoPWMNano {
    public:
        // Constructor: Defines the length of a full cycle in nanoseconds (mio of seconds)
        PicoPWMNano(uint64_t periodNanoSeconds){
            Logger.debug("PicoPWMNano");
            period_nano_sec = periodNanoSeconds;
            // the maximum number that the pwm_config_set_wrap & and pwm_set_chan_level supports is 65535
            rangeDivider = (1.0 * period_nano_sec) / PWM_MAX_NUMER;
        }

        // setup a pin for pwm write
        void begin(pin_size_t pin, uint64_t initialDutyCyleNanoSeconds=0){
            Logger.debug("PicoPWMNano::begin");
            if (setupConfig()){
                Logger.debug("gpio_set_function");
                gpio_set_function(pin, GPIO_FUNC_PWM);
                // determine the slice number and channel
                uint8_t slice_num = pwm_gpio_to_slice_num(pin);
                // uint8_t channel = pwm_gpio_to_channel(pinNumber);
                Logger.debug("pwm_init");
                pwm_init(slice_num, &config, true);
                setDutyCycle(pin, initialDutyCyleNanoSeconds);
            }
        }

        // sets the pin to low
        void end(pin_size_t pin){
            Logger.debug("PicoPWMNano::end");
            setDutyCycle(pin, 0);
            PinFunction.clear(pin);
        }

        // Defines the active period is nanoseconds
        void setDutyCycle(pin_size_t pin, uint64_t dutyCyleNanoSeconds){
            uint slice_num = pwm_gpio_to_slice_num(pin);
            uint channel = pwm_gpio_to_channel(pin);
            uint16_t value = (1.0 * dutyCyleNanoSeconds) / rangeDivider;

            if (Logger.isLogging(PicoLogger::Debug)){
                char str[40];
                sprintf(str,"%lu", dutyCyleNanoSeconds);
                Logger.debug("PWM duty cycle ns:",str);
                sprintf(str,"%lu", value);
                Logger.debug("PWM duty cycle(internal):",str);
            }
            pwm_set_chan_level(slice_num, channel, value);
        }

        // converts the indicated period is nanoseconds to hz 
        uint64_t frequency(uint64_t periodNanoSeconds){
            return 1000000000l / period_nano_sec;
        }

        // converts the PWM period to hz
        uint64_t frequency(){
            return frequency(period_nano_sec);
        }

        // provides the full cycle period in nanoseconds
        uint64_t period() {
            return period_nano_sec;
        }


    protected:
        uint64_t period_nano_sec;
        pwm_config config;
        float rangeDivider; // values must be <= 65535
        PicoPinFunction pinFunction = PicoPinFunction::instance();
        bool is_config_done = false;

        // provides the configuration - returns true if we have a new configuration
        bool setupConfig(){
            if (!is_config_done) {
                Logger.debug("PicoPWMNano::setupConfig");
                is_config_done = true;
                config = pwm_get_default_config();

                uint32_t freq =	clock_get_hz(clk_sys);
                float dividerClock =  (1.0 * freq) / frequency();  
                // divider for a full cycle
                float divider = dividerClock / PWM_MAX_NUMER;
                // this should actually be identical with PWM_MAX_NUMER
                uint16_t wrap = PWM_MAX_NUMER ;

                if (Logger.isLogging(PicoLogger::Debug)){
                    char str[80];
                    sprintf(str,"%ul", period_nano_sec);
                    Logger.debug("Period ns:", str);
                    sprintf(str,"%f", rangeDivider);
                    Logger.debug("Range divider:", str);
                    sprintf(str,"%d", frequency());
                    Logger.debug("PWM hz:", str);
                    sprintf(str,"%d", freq);
                    Logger.debug("Systemclock hz:", str);
                    sprintf(str,"%f", divider);
                    Logger.debug("PWM divider:", str);
                    sprintf(str,"%d", wrap);
                    Logger.debug("PWM wrap:", str);
                }
        
                // Set divider, reduces counter clock to sysclock/this value
                pwm_config_set_clkdiv(&config, divider);
                pwm_config_set_wrap(&config, wrap);
                return true;
            }
            return false;
        }
      
};


/**
 * @brief This is an even more powerfull PWM API where we can specify a user defined input range and the cycle frequency in hz.
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PicoPWM {
    public:
        PicoPWM(uint64_t frequency, uint64_t maxValue){
            // convert frequency to period 
            period_nano_sec = 1000000000l / frequency;
            nano = new PicoPWMNano(period_nano_sec);
            max_value = maxValue;
        }

        ~PicoPWM(){
            delete nano;
         }

        void begin(pin_size_t pin, uint64_t initalValue=0){
            nano->begin(pin, initalValue);
        }

        // sets the pin to low
        void end(pin_size_t pin){
            nano->end(pin);
        }

        // Defines the active period is nanoseconds
        void write(pin_size_t pin, uint64_t value){
            nano->setDutyCycle(pin, valueToDutyCycle(value));
        }

        // provides the full cycle period in nanoseconds
        uint64_t period() {
            return nano->period();
        }

        // converts the PWM period to hz
        uint64_t frequency(){
            return nano->frequency();
        }

    protected:
        PicoPWMNano *nano;
        int64_t max_value;
        uint64_t period_nano_sec;

        uint64_t valueToDutyCycle(uint64_t value){
            return map(value, 0, max_value, 0, period_nano_sec);
        }


};


