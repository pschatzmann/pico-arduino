#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define GPIO_FUNC_ADC 20

/**
 * @brief The pico requires that the function of the pin is defined. In Arduino, there is no such concept: so we do this at the first call of the 
 * corresponding function:
 * enum gpio_function {
    GPIO_FUNC_XIP = 0,
    GPIO_FUNC_SPI = 1,
    GPIO_FUNC_UART = 2,
    GPIO_FUNC_I2C = 3,
    GPIO_FUNC_PWM = 4,
    GPIO_FUNC_SIO = 5,
    GPIO_FUNC_PIO0 = 6,
    GPIO_FUNC_PIO1 = 7,
    GPIO_FUNC_GPCK = 8,
    GPIO_FUNC_USB = 9,
    GPIO_FUNC_NULL = 0xf,
};

 * @author Phil Schatzmann
 * @copyright GPLv3
 */


class PicoGPIOFunction {
    public:
        PicoGPIOFunction(int maxPins=40){
            pinInfo = new uint8_t[maxPins];
            for (int j=0; j<maxPins;j++){
                pinInfo[j] = GPIO_FUNC_NULL;
            }
        }

        // defines the actual pin function - only if it is necessary
        bool setFunction(pin_size_t pinNumber, gpio_function func){
            bool updated = false;
            if (pinInfo[pinNumber]!=func){
                Logger.info("gpio_set_function",Logger.toStr(pinNumber));
                gpio_set_function(pinNumber, func);
                updated = true;
            }
            return updated;
        }

        // ADC has a similar functionality but no gpio_function type
        void setFunctionADC(pin_size_t pinNumber) {
            Logger.debug("setFunctionADC");
            // init if necessary
            initADC();

            // setup pin for adc
            if (pinInfo[pinNumber] != GPIO_FUNC_ADC){
                Logger.info("adc_gpio_init",Logger.toStr(pinNumber));
                adc_gpio_init(pinNumber);
                pinInfo[pinNumber] = GPIO_FUNC_ADC;
            }

            // switch adc if necessary
            int adc = pinNumber - 26;
            if (current_adc != adc){
                Logger.info("adc_select_input",Logger.toStr(adc));
                adc_select_input((id_t)adc);
                current_adc = adc;
            }        
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

        // reads a pwm value
        int readPWM(int pinNumber, float scale=100.0){
             uint slice_num = pwm_gpio_to_slice_num(pinNumber);
            // Count once for every 100 cycles the PWM B input is high
            pwm_config cfg = pwm_get_default_config();
            pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_HIGH);
            pwm_config_set_clkdiv(&cfg, 100);
            pwm_init(slice_num, &cfg, false);
            gpio_set_function(pinNumber, GPIO_FUNC_PWM);

            pwm_set_enabled(slice_num, true);
            sleep_ms(10);
            pwm_set_enabled(slice_num, false);
            float counting_rate = clock_get_hz(clk_sys) / 100;
            float max_possible_count = counting_rate * 0.01;
            int result = pwm_get_counter(slice_num) / max_possible_count * scale;
            return result;
        }


    protected:
        uint8_t* pinInfo;
        bool adc_init_flag;
        int current_adc;


};