#include "Arduino.h"
#include "PluggableUSB.h"
#include "PicoLogger.h"
#include "PicoHardwareSerial.h"
#include "PicoTone.h"

// public data
PicoDefaultSerial Serial;
PicoHardwareSerial Serial1(UART0_BASE, 0);
PicoHardwareSerial Serial2(UART1_BASE, 1); 
PicoLogger Logger;    // Support for logging
PicoTone ToneImpl;    // Sound support 
PicoHardwareSPI SPI(spi0);
PicoHardwareSPI SPI1(spi1);
PicoHardwareI2C Wire; // I2C 

// local data
static bool adc_init_flag = false;


// sleep ms milliseconds
void delay(unsigned long ms){
    sleep_ms(ms);
}

// sleep us milliseconds
void delayMicroseconds(unsigned int us){
    sleep_us(us);
}

// Returns the number of milliseconds passed since epoch
unsigned long millis() {
    absolute_time_t t = get_absolute_time();
    return to_ms_since_boot (t);
}

// Returns the micros of milliseconds passed since epoch
unsigned long micros(void) {
    absolute_time_t t = get_absolute_time();
    return to_us_since_boot(t);
}

void pinMode(pin_size_t pinNumber, PinMode pinMode){
    gpio_init(pinNumber);
    gpio_set_dir(pinNumber, pinMode==OUTPUT ? GPIO_OUT:  GPIO_IN);
    if (pinMode==INPUT_PULLUP)
        gpio_pull_up(pinNumber);
    if (pinMode==INPUT_PULLDOWN)
        gpio_pull_down(pinNumber);
}

void digitalWrite(pin_size_t pinNumber, PinStatus status) {
    gpio_put(pinNumber, status==0 ? LOW : HIGH);
}

PinStatus digitalRead(pin_size_t pinNumber) {
    int value = gpio_get(pinNumber);
    return value==0 ? LOW : HIGH;
}

// setup the analog to digital converter the first time we use it
void initADC(){
    // init if necessary
    if (adc_init_flag){
        adc_init();
        adc_init_flag = true;
    }
}

/**
 * @brief User ADC inputs are on 0-3 (GPIO 26-29), the temperature sensor is on input 4.
 * 
 * @param pinNumber 
 * @return int 
 */
int analogRead(int pinNumber){
    static int current_adc = -1;
    static bool adc_init_pin_flag[5];
    if (pinNumber>=26 && pinNumber<=29){
        // init if necessary
        initADC();
        int id = pinNumber - 26;
        // setup pin for adc
        if (!adc_init_pin_flag[id]){
            adc_gpio_init(pinNumber);
            adc_init_pin_flag[id] = true;
        }

        // switch pin if necessary
        if (current_adc!=id){
            adc_select_input(id);
        }
        return adc_read();
    } else {
        Logger.error("Invalid analog pin (expected values 26-29) :", String(pinNumber).c_str());
    }
    return -1;
}

int getTemperature(){
    initADC();
    //Input 4 is the onboard temperature sensor
    adc_select_input(4);
    int value = adc_read();
    int temperature = 27 - (value - 0.706)/0.001721;
    return temperature;
}

void analogReference(uint8_t mode){

}

void analogWrite(pin_size_t pinNumber, int value) {
   gpio_set_function(pinNumber, GPIO_FUNC_PWM);   
   pwm_set_gpio_level(pinNumber, value);
}

int64_t stop_tone_callback(alarm_id_t id, void *user_data) {
    // ugliy hack: we receive the pin number in user_data (not a pointer!)
    uint32_t pin_ptr = (uint32_t) user_data;
    noTone(pin_ptr);
    return 0;
 }

void tone(uint8_t pinNumber, unsigned int frequency, unsigned long duration) {
    ToneImpl.tone(pinNumber,frequency);
    add_alarm_in_ms(duration, stop_tone_callback, (void*) pinNumber , false);
}

void noTone(uint8_t pinNumber) {    
    ToneImpl.noTone(pinNumber);
}

// ls
// actually does nothing
void yield(void){
}

//static PluggableUSB_ obj;
PluggableUSB_::PluggableUSB_(){}



