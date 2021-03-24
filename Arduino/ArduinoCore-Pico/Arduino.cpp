/**
 * Arduino.cpp - Main implementation file for the PICO Arduino SDK.
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
#include "hardware/clocks.h"
#include "PicoPWM.h"


// Standard Arduino global variables
#if !defined(TINYUSB_HOST_LINKED) && !defined(TINYUSB_DEVICE_LINKED)
PicoSerialUSB Serial;
#endif
PicoSerialUART Serial1(0);
PicoSerialUART Serial2(1); 
PicoHardwareSPI SPI(spi0);
PicoHardwareSPI SPI1(spi1);
PicoHardwareI2C Wire(i2c0, 160, I2C_SDA, I2C_SCL);  
PicoHardwareI2C Wire1(i2c1, 160, I2C1_SDA, I2C1_SCL);  

//Pico Framework global variables 
PicoPinFunction PinFunction = PicoPinFunction::instance();
PicoLogger Logger;    // Support for logging
PicoPWM ArduionPwm(PICO_ARDUINO_PWM_FREQUENCY, 255); // pwm support
PinSetupGPIO InstancePinSetupGPIO;
PinSetupADC InstancePinSetupADC = PinSetupADC::instance();

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
    // register function and mode
    PinFunction.setPinMode(pinNumber, pinMode);
}

void digitalWrite(pin_size_t pinNumber, PinStatus status) {
    PinFunction.usePin(pinNumber, PIN_FUNC_GPIO, &InstancePinSetupGPIO);
    gpio_put(pinNumber, status==0 ? LOW : HIGH);
}

PinStatus digitalRead(pin_size_t pinNumber) {
    PinFunction.usePin(pinNumber, PIN_FUNC_GPIO, &InstancePinSetupGPIO);
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
    Logger.debug("analogRead");
    // analog read
    if (pinNumber>=26 && pinNumber<=29){
        // ADC
        PinFunction.usePin(pinNumber, PIN_FUNC_ADC, &InstancePinSetupADC);
        return adc_read();
    } else {
        // PWM Read
        ArduionPwm.begin(pinNumber);
        return ArduionPwm.read(pinNumber);
    }
    return -1;
}

// reads the on board temparature in C 
int temperature(){
    // init ADC only if it has not been set up yet
    PinSetupADC::instance().initADC();
    // this is just setting some flags - so it does not hurt to call this redundantly
    adc_set_temp_sensor_enabled(true);

    //Input 4 is the onboard temperature sensor - operation only if there is a change
    PinSetupADC::instance().adcSelect(4);
    int value = 0;
    for (uint8_t i = 0; i < 30 ;i++)
    {
        value = (value + adc_read())/2;
    }
    //T = 27 - (ADC_Voltage - 0.706)/0.001721
    //The ADC_Voltage is equal to adc_read() * 3.3 / (1<<12)
    const float conversion_factor = 3.3f / (1<<12);
    int temperature = 27 - (value * conversion_factor - 0.706)/0.001721;
    return temperature;
}

// reads the on board temparature in F for your Amerian friends
int temperatureF(){
    return (static_cast<float>(temperature()) * 9.0/5.0) + 32;
}

// Not implemented for the Pico
void analogReference(uint8_t mode){
    Logger.error("analogReference not implemented!");
}

// Writes an analog value (PWM wave) to a pin. The value needs to be between 0 and 255
void analogWrite(pin_size_t pinNumber, int value) {
    ArduionPwm.begin(pinNumber);
    uint64_t val64 = 0;
    if (value>0){
        val64 = value;
    }
    ArduionPwm.write(pinNumber, val64);
}

// Plays a tone
void tone(pin_size_t pinNumber, unsigned int frequency, unsigned long duration) {
    ArduinoPicoTone::tone(pinNumber, frequency, duration);
}

// stops a tone
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


