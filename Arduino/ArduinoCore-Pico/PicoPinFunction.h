#pragma once

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#ifndef PICO_MAX_PIN_COUNT 
#define PICO_MAX_PIN_COUNT 40
#endif

namespace pico_arduino {

/// Supported pin functions
enum PinFunctionEnum {PIN_FUNC_UNDEFINED, PIN_FUNC_GPIO, PIN_FUNC_ADC, PIN_FUNC_PWM};

// Forward declaraton
class PinSetup;

/**
 * @brief Information about an the status and the Arduino PinMode of an individual pin
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct PinInfo {
    PinFunctionEnum pin_function;
    PinMode pinMode;
    bool is_setup = false;
    bool is_defined = false;
};

/**
 * @brief Base class for function specific pin setup and pin use functionality 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinSetup {
public:
    virtual void setupPin(PinInfo *info, pin_size_t pin) = 0;
    virtual bool usePin(PinInfo *info, pin_size_t pin) { return false; };
};

/**
 * @brief Pin Initialization for ADC
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinSetupADC : public PinSetup {
public:

    static PinSetupADC &instance() {
        static PinSetupADC *ptr = new PinSetupADC();
        return *ptr;
    }

    virtual void setupPin(PinInfo *info, pin_size_t pinNumber) {
        Logger.debug("PinSetupADC::setupPin");
        initADC();
    }

    virtual bool usePin(PinInfo *info, pin_size_t pinNumber) {
        int adc = pinNumber - 26;
        return adcSelect(adc);
    };


    /// select the ADC if it has been changed
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

    /// calls adc_init() if necessary - returns true if it has been intialized (the first time)
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

protected:
    bool adc_init_flag = false;
    int current_adc;

    // singleton ! use the instance method to get the object
    PinSetupADC() {
    }

};

/**
 * @brief Pin Initialization for GPIO
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PinSetupGPIO : public PinSetup {
public:
    virtual void setupPin(PinInfo *info, pin_size_t pinNumber) {
        Logger.debug("PinSetupGPIO::setupPin");
        gpio_init(pinNumber);
        gpio_set_dir(pinNumber, info->pinMode==OUTPUT ? GPIO_OUT:  GPIO_IN);
        if (info->pinMode==INPUT_PULLUP) gpio_pull_up(pinNumber);
        if (info->pinMode==INPUT_PULLDOWN) gpio_pull_down(pinNumber);
        info->is_setup = true;
    }
};


/**
 * @brief The pico requires that the function of the pin is defined. In Arduino, there is no such concept - however we need to define if the pin
 * is input or output
 *
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class PicoPinFunction {
    public:
        static PicoPinFunction &instance(){
            static PicoPinFunction *inst_ptr;
            if (inst_ptr==nullptr){
                inst_ptr = new PicoPinFunction();
            }
            return *inst_ptr;
        }

        /// defines the actual Arduino PinMode. Returns true if it needed to be changed
        bool setPinMode(pin_size_t pinNumber, PinMode pinMode){
            Logger.debug("PicoGPIOFunction::PinMode");
            bool changed = false;
            if (!(pinInfo[pinNumber].is_defined) || pinInfo[pinNumber].pinMode != pinMode){
                pinInfo[pinNumber].pinMode = pinMode;
                pinInfo[pinNumber].is_setup = false;
                pinInfo[pinNumber].is_defined = true;

                Logger.debug("PicoGPIOFunction::PinMode -> has changed", Logger.toStr(pinNumber));
                Logger.debug("PicoGPIOFunction::PinMode ->",pinModeStr(pinNumber));
                changed = true;
            }
            return changed;
        }

        /// checks if the mode was defined for the pin
        bool isModeDefined(pin_size_t pinNumber) {
            return pinInfo[pinNumber].is_defined;
        }

        /// checks if the pin has been defined as input
        bool isInput(pin_size_t pinNumber){
            return pinInfo[pinNumber].pinMode==INPUT;
        }

        /// checks if the pin has been defined as output
        bool isOutput(pin_size_t pinNumber){
            return pinInfo[pinNumber].pinMode==OUTPUT;
        }

        /// checks if the setup has been done for the pin
        bool isSetup(pin_size_t pinNumber){
            return pinInfo[pinNumber].is_setup;
        }

        /// marks the pin setup as completed
        void setSetup(pin_size_t pinNumber){
            pinInfo[pinNumber].is_setup = true;
        }

        /// returns the pin mode
        PinMode pinMode(pin_size_t pinNumber) {
            return (PinMode) pinInfo[pinNumber].pinMode;
        }

        /// returns the PinMode as string
        const char* pinModeStr(pin_size_t pinNumber){
            if (isModeDefined(pinNumber)) {
                return isInput(pinNumber) ? "INPUT" : "OUTPUT";
            } else {
                return "UNDEFINED";
            }
        }

        /// set gpio function to GPIO_FUNC_NULL 
        void clear(pin_size_t pinNumber){
            Logger.debug("clear");
            gpio_set_function(pinNumber, GPIO_FUNC_NULL);
            pinInfo[pinNumber].is_setup = false;
        }

        /// setup Pico pin init function bysed on functionality
        void usePin(pin_size_t pinNumber, PinFunctionEnum pinFunction, PinSetup *setup = nullptr){
            Logger.debug("PicoGPIOFunction::usePin", Logger.toStr(pinNumber));
            PinInfo *info = & (pinInfo[pinNumber]);
            //Logger.debug("is_setup:", pinInfo[pinNumber].is_setup ? "true" : "false");
            if (!info->is_setup) {
                // initialize setup method
                if (setup!=nullptr){
                    setup->setupPin(info, pinNumber);
                    info->is_setup = true;
                }
            }

            // fuctionality which needs to be executed each time before the pin use
            if (setup!=nullptr){
                setup->usePin(info, pinNumber);
            }
        }

    protected:
        PinInfo* pinInfo;
        bool adc_init_flag = false;
        int current_adc;

        /// protected constructor because this is a singleton
        PicoPinFunction(){
            Logger.debug("PicoGPIOFunction");
            pinInfo = new PinInfo[PICO_MAX_PIN_COUNT];
        }
};

}

