/**
 * Arduino.cpp - Main implementation file for the PICO Arduino SDK
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 */


#include "Arduino.h"
#include "Wire.h"
#include "PluggableUSB.h"
#include "PicoLogger.h"
#include "PicoHardwareSerial.h"
#include "PicoTone.h"
#include "hardware/watchdog.h"


// public data
PicoDefaultSerial Serial;
PicoHardwareSerial Serial1(0);
PicoHardwareSerial Serial2(1); 
PicoHardwareSPI SPI(spi0);
PicoHardwareSPI SPI1(spi1);
PicoGPIOFunction GPIOFunction;
PicoHardwareI2C Wire(i2c0, 160, GP12, GP13);  
PicoHardwareI2C Wire1(i2c1, 160, GP14, GP15);  
PicoLogger Logger;    // Support for logging

// local data
static bool adc_init_flag = false;


// sleep ms milliseconds
void delay(unsigned long ms){
    sleep_ms(ms);
}

// sleep us microseconds
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


/**
 * @brief User ADC inputs are on 0-3 (GPIO 26-29), the temperature sensor is on input 4.
 * Uses PWM on other pins
 * @param pinNumber 
 * @return int 
 */
int analogRead(pin_size_t pinNumber){
    // analog read
    if (pinNumber>=26 && pinNumber<=29){
        // ADC
        GPIOFunction.setFunctionADC(pinNumber);
        return adc_read();
    } else {
        if (pwm_gpio_to_channel(pinNumber) == PWM_CHAN_B){
            return GPIOFunction.readPWM(pinNumber, 100.0);
        } else {
            Logger.error("Invalid analog pin (expected values 26-29) :", String(pinNumber).c_str());
            Logger.error("pwm is expected on PWM_CHAN_B - currently used pin:", String(pinNumber).c_str());
        }
    }
    return -1;
}

// reads the on board temparature
int getTemperature(){
    GPIOFunction.initADC();
    //Input 4 is the onboard temperature sensor
    adc_select_input(4);
    int value = adc_read();
    int temperature = 27 - (value - 0.706)/0.001721;
    return temperature;
}

void analogReference(uint8_t mode){

}

// writes PWM
void analogWrite(pin_size_t pinNumber, int value) {
    GPIOFunction.setFunction(pinNumber, GPIO_FUNC_PWM);
    pwm_set_gpio_level(pinNumber, value);
}


void tone(pin_size_t pinNumber, unsigned int frequency, unsigned long duration) {
    ArduinoPicoTone::tone(pinNumber, frequency, duration);
}

void noTone(pin_size_t pinNumber) {    
    ArduinoPicoTone::noTone(pinNumber);
}

// actually does nothing
void yield(void){
}

//static PluggableUSB_ obj;
PluggableUSB_::PluggableUSB_(){}

// define Arduino setup(()) and loop()
int main() {
    setup();
    while(true){
        loop();
        watchdog_update();
    }
}


