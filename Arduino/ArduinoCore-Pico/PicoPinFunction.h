#pragma once

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

/**
 * @brief The pico requires that the function of the pin is defined. In Arduino, there is no such concept: so we do this at the first call of the 
 * corresponding function.

 * @author Phil Schatzmann
 * @copyright GPLv3
 */

enum PinFunction {PIN_FUNC_UNDEFINED, PIN_FUNC_GPIO, PIN_FUNC_ADC, PIN_FUNC_PWM};

struct PinInfo {
    PinFunction pin_function;
    PinMode pinMode;
    bool is_setup = false;
};

class PicoPinFunction {
    public:
        static PicoPinFunction &instance(){
            static PicoPinFunction *inst_ptr;
            if (inst_ptr==nullptr){
                inst_ptr = new PicoPinFunction();
            }
            return *inst_ptr;
        }

        // defines the actual Arduino PinMode
        bool setPinMode(pin_size_t pinNumber, PinMode pinMode=INPUT){
            Logger.debug("PicoGPIOFunction::PinMode");
            bool changed = false;
            if ( pinInfo[pinNumber].pinMode != pinMode){
                Logger.debug("PicoGPIOFunction::PinMode -> has changed");
                pinInfo[pinNumber].pinMode = pinMode;
                pinInfo[pinNumber].is_setup = false;
                changed = true;
            }
            return changed;
        }

        bool isInput(pin_size_t pinNumber){
            return pinInfo[pinNumber].pinMode==INPUT;
        }

        bool isOutput(pin_size_t pinNumber){
            return pinInfo[pinNumber].pinMode==OUTPUT;
        }

        void clear(pin_size_t pinNumber){
            Logger.debug("clear");
            gpio_set_function(pinNumber, GPIO_FUNC_NULL);
            pinInfo[pinNumber].is_setup = false;
        }

        // setup Pico pin init function bysed on functionality
        void usePin(pin_size_t pinNumber, PinFunction pinFunction){
            Logger.debug("PicoGPIOFunction::usePin", Logger.toStr(pinNumber));
            PinInfo *info = & (pinInfo[pinNumber]);
            Logger.debug("is_setup:", pinInfo[pinNumber].is_setup ? "true" : "false");
            if (!info->is_setup) {
                Logger.debug("PicoGPIOFunction::usePin");
                // just define the function
                info->pin_function = pinFunction;
                switch(info->pin_function){

                    case PIN_FUNC_GPIO:
                        Logger.debug("setup PIN_FUNC_GPIO");
                        gpio_init(pinNumber);
                        gpio_set_dir(pinNumber, info->pinMode==OUTPUT ? GPIO_OUT:  GPIO_IN);
                        if (info->pinMode==INPUT_PULLUP) gpio_pull_up(pinNumber);
                        if (info->pinMode==INPUT_PULLDOWN) gpio_pull_down(pinNumber);
                        info->is_setup = true;
                        break;

                    case PIN_FUNC_ADC:
                        Logger.debug("setup PIN_FUNC_ADC");
                        initADC();
                        adc_gpio_init(pinNumber);
                        info->is_setup = true;
                        break;

                    default:
                         Logger.warning("unsupported pin function");
                         break;

                }
            }

            // for the PIN_FUNC_ADC we might need to swith the adc
            if (info->pin_function==PIN_FUNC_ADC){
                int adc = pinNumber - 26;
                adcSelect(adc);
            }

        }

        // select the ADC if it has been changed
        bool adcSelect(int adc){
            bool changed = false;
            if (current_adc != adc){
                Logger.debug("adc_select_input",Logger.toStr(adc));
                adc_select_input((id_t)adc);
                current_adc = adc;
                changed = true;
            } 
            return changed;                   
        }

        // calls adc_init() if necessary - returns true if it has been intialized (the first time)
        bool initADC(){
            // init if necessary
            bool result = false;
            if (!adc_init_flag){
                Logger.info("adc_init");
                adc_init();
                adc_init_flag = true;
                result = true;
            }
            return result;
        }

        void setPwmConfig(pwm_config config){
            this->pwmconfig = config;
        }

    protected:
        PinInfo* pinInfo;
        bool adc_init_flag;
        int current_adc;
        pwm_config pwmconfig;


        PicoPinFunction(int maxPins=40){
            Logger.debug("PicoGPIOFunction");
            pinInfo = new PinInfo[maxPins];
        }
};

