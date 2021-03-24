#pragma once

#include "pico/stdlib.h"
#include <inttypes.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "PicoPinFunction.h"

#ifndef PWM_MAX_NUMER
// 65535
#define PWM_MAX_NUMER 1000
#endif

#ifndef PWM_READ_REPEAT
#define PWM_READ_REPEAT 10
#endif


/// For measureDutyCycle we need to track the pwm wrap interrupts
inline uint32_t pico_pwm_wrap_count;
inline int pico_pwm_actual_pin;
inline static void on_pico_pwm_wrap() {
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(pico_pwm_actual_pin));
    // increment the wrap counter
    pico_pwm_wrap_count++;
};

/// converts the indicated period is nanoseconds to hz 
inline static float frequency(uint64_t period_nano_sec){
    return 1000000000.0 / period_nano_sec;
}


/**
 * @brief Support for the generation of PWM signals.
 * For the standard Arduino functionality we use on fixed frequency (defined in the constructor). If you use this
 * API you can define separate frequencies per slice by creating separate instances.
 * 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class PicoPWMWriter : public PinSetup {
    public:
        /// Constructor: Defines the length of a full cycle in nanoseconds (mio of seconds)
        PicoPWMWriter(uint64_t periodNanoSeconds){
            Logger.debug("PicoPWMWriter");
            period_nano_sec = periodNanoSeconds;
            tick_period_nano_sec =  period_nano_sec / PWM_MAX_NUMER;
        }

        /// setup a pin for pwm write
        void begin(pin_size_t pin, uint64_t initialDutyCyleNanoSeconds=0){
            Logger.debug("PicoPWMWriter::begin", Logger.toStr(pin));
            initial_duty_cycle = initialDutyCyleNanoSeconds;
            // this is executed only once per instance
            setupConfig();
            // this is executed only once per pin
            pinFunction.usePin(pin, PIN_FUNC_PWM, this);
        }

        /// setup of pin for PWM Output
        virtual void setupPin(PinInfo *info, pin_size_t pinNumber) {
            Logger.debug("PicoPWMWriter::setupPin");
            // determine the slice number and channel
            uint8_t slice_num = pwm_gpio_to_slice_num(pinNumber);

            // start only in output mode
            Logger.debug("pwm_init for  OUTPUT");
            pwm_init(slice_num, &config, true);
            setDutyCycle(pinNumber, initial_duty_cycle);

            Logger.debug("gpio_set_function");
            gpio_set_function(pinNumber, GPIO_FUNC_PWM);
        }

        /// sets the pin to low
        void end(pin_size_t pin){
            Logger.debug("PicoPWMWriter::end");
            setDutyCycle(pin, 0);
            PinFunction.clear(pin);
        }

        /// Defines the active period is nanoseconds
        void setDutyCycle(pin_size_t pin, uint64_t dutyCyleNanoSeconds){
            Logger.debug("PicoPWMWriter::setDutyCycle");
            uint slice_num = pwm_gpio_to_slice_num(pin);
            uint channel = pwm_gpio_to_channel(pin);
            uint16_t value = (1.0 * dutyCyleNanoSeconds) / tick_period_nano_sec;

            if (Logger.isLogging(PicoLogger::Debug)){
                char str[40];
                sprintf(str,"%lu", dutyCyleNanoSeconds);
                Logger.debug("PWM duty cycle ns:",str);
                sprintf(str,"%lu", value);
                Logger.debug("PWM duty cycle(internal):",str);
            }
            pwm_set_chan_level(slice_num, channel, value);
        }

        /// converts the PWM period to hz
        float frequency(){
            return ::frequency(period_nano_sec);
        }

        // frequency of a single counter tick
        float frequencyTick(){
            return ::frequency(tick_period_nano_sec);
        }

        // period of a single tick in nanoseconds
        float periodTick(){
            return tick_period_nano_sec;
        }

        /// provides the full cycle period in nanoseconds
        uint64_t period() {
            return period_nano_sec;
        }

    protected:
        PicoPinFunction pinFunction = PicoPinFunction::instance();
        PinMode pin_mode;
        pwm_config config;
        uint64_t period_nano_sec;
        uint64_t initial_duty_cycle;
        float tick_period_nano_sec; // values must be <= 65535
        bool is_config_done = false;

        /// provides the configuration - returns true if we have a new configuration
        bool setupConfig(){
            if (!is_config_done) {
                Logger.debug("PicoPWMWriter::setupConfig");
                is_config_done = true;
                uint16_t wrap = PWM_MAX_NUMER ;
                config = pwm_get_default_config();
                // this should actually be identical with PWM_MAX_NUMER
                uint32_t sys_clock_freq = clock_get_hz(clk_sys);
                // divider to achieve the requested periodNanoSeconds 
                float dividerTick =  (1.0 * sys_clock_freq) / frequencyTick();  
                logConfig(sys_clock_freq, dividerTick, wrap);

                // Set divider, reduces counter clock to sysclock/this value
                pwm_config_set_clkdiv(&config, dividerTick);
                pwm_config_set_wrap(&config, wrap);

                return true;
            }
            return false;
        }   

        void logConfig(uint32_t sys_clock_freq, float dividerTick, uint16_t wrap ) {
            if (Logger.isLogging(PicoLogger::Debug)){
                char str[80];
                sprintf(str,"%lu", period_nano_sec);
                Logger.debug("Period ns:", str);
                sprintf(str,"%f", tick_period_nano_sec);
                Logger.debug("Tick period ns:", str);
                sprintf(str,"%f", frequency());
                Logger.debug("PWM hz:", str);
                sprintf(str,"%f", sys_clock_freq);
                Logger.debug("Systemclock hz:", str);
                sprintf(str,"%f", dividerTick);
                Logger.debug("Tick divider:", str);
                sprintf(str,"%d", wrap);
                Logger.debug("PWM wrap:", str);
            }
        }


};

/**
 * @brief PWM class which supports the input of PWM signals
 * 
 */
class PicoPWMReader : public PinSetup {
    public:                 
        /**
         * @brief Constructor: Defines the length of a full cycle in nanoseconds (mio of seconds). This is used as 
         * mesurement period.
         * @param periodNanoSeconds 
         */
        
        PicoPWMReader(uint64_t periodNanoSeconds){
            Logger.debug("PicoPWMReader");
            period_nano_sec = periodNanoSeconds;
            tick_period_nano_sec =  period_nano_sec / PWM_MAX_NUMER;
        }

        /// setup a pin for pwm write
        void begin(pin_size_t pinNumber){
            Logger.debug("PicoPWMReader::begin", Logger.toStr(pinNumber));

            // this is executed only once per instance
            setupConfig();

            // force input mode
            pinFunction.setPinMode(pinNumber, INPUT);
            pin_mode = pinFunction.pinMode(pinNumber);

            // this is executed only once per pin
            pinFunction.usePin(pinNumber, PIN_FUNC_PWM, this);
        }

        /// setup of pin for PWM Input
        virtual void setupPin(PinInfo *info, pin_size_t pinNumber) {
            Logger.debug("PicoPWMReader::setupPin");
            // determine the slice number and channel
            uint8_t slice_num = pwm_gpio_to_slice_num(pinNumber);

            // start only in output mode
            Logger.debug("pwm_init for INPUT");
            // READ Mode -> count high !
            pwm_config_set_clkdiv_mode(&config, PWM_DIV_B_HIGH);
            pwm_init(slice_num, &config, false);

            Logger.debug("gpio_set_function");
            gpio_set_function(pinNumber, GPIO_FUNC_PWM);
        }

        /// sets the pin to low
        void end(pin_size_t pin){
            Logger.debug("PicoPWMReader::end");
            PinFunction.clear(pin);
        }

        /// measures the duty cycle (active period) in nanoseconds - only the PWM B pins can be used as inputs!
        uint64_t measureDutyCycle(uint gpio) {
            return measureDutyCyclePercent(gpio) *  period() / 100.0 ;
        }

        /// provides the duty cycle in percent
        float measureDutyCyclePercent(uint gpio) {
            Logger.debug("PicoPWMReader::measureDutyCycle");
            float result = 0;
            if (pin_mode==INPUT) {
                // Only the PWM B pins can be used as inputs.
                if(pwm_gpio_to_channel(gpio) == PWM_CHAN_B) {
                    // used by interrupt handler
                    pico_pwm_actual_pin = gpio;
                    uint slice_num = pwm_gpio_to_slice_num(gpio);

                    // Mask our slice's IRQ output into the PWM block's single interrupt line,
                    // and register our interrupt handler
                    pwm_clear_irq(slice_num);
                    pwm_set_irq_enabled(slice_num, true);
                    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pico_pwm_wrap);
                    irq_set_enabled(PWM_IRQ_WRAP, true);

                    // measure input pin
                    uint64_t counter = count(slice_num, PWM_READ_REPEAT);

                    // clean up irq
                    pwm_clear_irq(slice_num);
                    pwm_set_irq_enabled(slice_num, false);
                    irq_remove_handler (PWM_IRQ_WRAP, on_pico_pwm_wrap);

                    // convert to percent
                    result =  100.0 * counter / max_ticks_per_period;
                } else {
                    Logger.error("measureDutyCycle() only allowed the PWM B pins");
                }
            } else {
                Logger.error("measureDutyCycle() only allowed with pinMode INPUT");
            }
            return result;
        }

        /// provides the full cycle period in nanoseconds
        uint64_t period() {
            return period_nano_sec;
        }


    protected:
        PicoPinFunction pinFunction = PicoPinFunction::instance();
        PinMode pin_mode;
        pwm_config config;
        uint64_t period_nano_sec;
        uint64_t max_ticks_per_period;
        float tick_period_nano_sec; // values must be <= 65535
        bool is_config_done = false;


        /// provides the configuration - returns true if we have a new configuration
        bool setupConfig(){
            if (!is_config_done) {
                Logger.debug("PicoPWMReader::setupConfig");
                is_config_done = true;
                uint16_t wrap = PWM_MAX_NUMER ;
                config = pwm_get_default_config();
                // this should actually be identical with PWM_MAX_NUMER
                uint32_t sys_clock_freq = clock_get_hz(clk_sys);

                // calculate max ticks per period -> sys_clock_freq
                max_ticks_per_period = period_nano_sec * sys_clock_freq / 1000000000; 

                // Set divider, run at full speed
                pwm_config_set_clkdiv(&config, 1.0);
                pwm_config_set_wrap(&config, wrap);

                logConfig(sys_clock_freq, wrap);
                return true;
            }
            return false;
        }

        void logConfig(uint32_t sys_clock_freq, uint16_t wrap ) {
            if (Logger.isLogging(PicoLogger::Debug)){
                char str[80];
                sprintf(str,"%lu", period_nano_sec);
                Logger.debug("Period ns:", str);
                sprintf(str,"%f", tick_period_nano_sec);
                Logger.debug("Tick period ns:", str);
                sprintf(str,"%f", sys_clock_freq);
                Logger.debug("Systemclock hz:", str);
                sprintf(str,"%d", wrap);
                Logger.debug("PWM wrap:", str);
            }
        }

        uint64_t count(uint slice_num, int repeat) {
            uint64_t sleep_period_us =  (period_nano_sec / 1000 );  // 2040
            pico_pwm_wrap_count = 0;

            if (Logger.isLogging(PicoLogger::Debug)){
                char sleep_str[70];
                sprintf(sleep_str,"sleep_us: %lu", sleep_period_us);
                Logger.debug(sleep_str);
            }

            uint64_t counter = 0;
            for (int j=0;j<repeat;j++) {
                // measure n pwm cycles
                pwm_set_counter (slice_num, 0);
                pwm_set_enabled(slice_num, true);
                sleep_us((sleep_period_us )); // sleep microsendonds
                pwm_set_enabled(slice_num, false);

                counter +=  pwm_get_counter(slice_num);
            }
            counter += (pico_pwm_wrap_count * PWM_MAX_NUMER);
            return counter / repeat;
        }

};

/**
 * @brief The Raspberry Pico has 8 controllable PWM slices with each 2 channels, so we can control up to 16 pwm devices at the same time!
 * 
 * This is Simple Basic PWM API where we specifiy the periods and duty cyle in Nanoseconds. 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class PicoPWMNano {
    public:
        /// Constructor: Defines the length of a full cycle in nanoseconds (mio of seconds)
        PicoPWMNano(uint64_t periodNanoSeconds) {
            Logger.debug("PicoPWMNano");
            writer = new PicoPWMWriter(periodNanoSeconds);
            reader = new PicoPWMReader(periodNanoSeconds);
        }

        // Destrucotor - clean up 
        ~PicoPWMNano(){
            delete writer;
            delete reader;
        }

        /// setup the pin mode only if necessary
        void begin(pin_size_t gpio, uint64_t initialDutyCyleNanoSeconds=0, PinMode pinMode=OUTPUT){
            // determine PinMode from PinFunction - otherwise we use the indicated PinMode
            if (!pin_function.isModeDefined(gpio)){
                pin_function.setPinMode(gpio, pinMode);
            } 

            if (pin_function.isInput(gpio)){
                reader->begin(gpio);
            } else if (pin_function.isOutput(gpio)){
                writer->begin(gpio, initialDutyCyleNanoSeconds);
            }
        }

        /// sets the output pins to low
        void end(pin_size_t gpio){
            writer->end(gpio);
            reader->end(gpio);
        }

        /// Defines the active period is nanoseconds
        void setDutyCycle(pin_size_t gpio, uint64_t dutyCyleNanoSeconds){
            if (isOutput(gpio)){
              writer-> setDutyCycle(gpio, dutyCyleNanoSeconds);
            }
        }

        /// measures the duty cycle in nanoseconds - only the PWM B pins can be used as inputs!
        uint64_t measureDutyCycle(uint gpio) {
            uint64_t result = 0;
            if (isInput(gpio)) {
                result = reader->measureDutyCycle(gpio);
            }
            return result;
        }

        /// provides the duty cycle in percent
        float measureDutyCyclePercent(uint gpio) {
            float result = 0;
            if (isInput(gpio)) {
                result = reader->measureDutyCyclePercent(gpio);
            }
            return result;
        }

        /// provides the duty cycle in percent
        float measurePeriod(uint gpio) {
            float result = 0;
            if (isInput(gpio)) {
                result = reader->period();
            }
            return result;
        }

        /// converts the PWM period to hz for the PWM output
        float frequency(){
            return writer->frequency();
        }

        /// provides the full cycle period in nanoseconds for the PWM output
        uint64_t period() {
            return  writer->period();
        }

        /// Utility function: converts the indicated period is nanoseconds to hz 
        static float frequency(uint64_t periodNanoSeconds){
            return 1000000000.0 / periodNanoSeconds;
        }


    protected:
        PicoPinFunction  pin_function = PicoPinFunction::instance();
        PinMode pin_mode;
        PicoPWMWriter *writer;
        PicoPWMReader *reader;

        bool isOutput(pin_size_t gpio) {
            return pin_function.isOutput(gpio);
        }
        bool isInput(pin_size_t gpio) {
            return pin_function.isInput(gpio);
        }

};


/**
 * @brief This is an even more powerfull PWM API where we can specify a user defined input range and the cycle frequency in hz.
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PicoPWM {
    public:
        /// Default constructor
        PicoPWM(uint64_t frequency, uint64_t maxValue){
            // convert frequency to period 
            period_nano_sec = 1000000000l / frequency;
            nano = new PicoPWMNano(period_nano_sec);
            max_value = maxValue;
        }

        /// Destructor
        ~PicoPWM(){
            delete nano;
         }

        /// setup a pin for pwm write
        void begin(pin_size_t pin, uint64_t initalValue=0){
            nano->begin(pin, initalValue);
        }

        /// sets the pin to low
        void end(pin_size_t pin){
            nano->end(pin);
        }

        /// Defines the active period in the value range from 0 to maxValue
        void write(pin_size_t pin, uint64_t value){
            nano->setDutyCycle(pin, valueToDutyCycle(value));
        }

        /// Reads the active period in the value range from 0 to maxValue
        uint64_t read(pin_size_t pin){
            return dutyCycleToValue(nano->measureDutyCycle(pin));
        }

        /// Provides the duty cyle in percent
        float readPercent(pin_size_t pin){
            return 100.0 * read(pin) / max_value;
        }

        /// provides the full cycle period in nanoseconds
        uint64_t period() {
            return nano->period();
        }

        /// converts the PWM period to hz
        uint64_t frequency(){
            return nano->frequency();
        }

    protected:
        PicoPWMNano *nano;
        int64_t max_value;
        uint64_t period_nano_sec;

        /// converts an input value to the duty cycle in nanosec
        uint64_t valueToDutyCycle(uint64_t value){
            return map(value, 0, max_value, 0, period_nano_sec);
        }

        uint64_t dutyCycleToValue(uint64_t dutyCycle){
            return map(dutyCycle, 0, period_nano_sec, 0, max_value) + 1;
        }

};


