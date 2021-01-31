#pragma once
#include "HardwareI2C.h"

/**
 * @brief Arduino I2C implementation using the Pico functionality 
 * 
 */
class PicoHardwareI2C : public HardwareI2C {
  public:
    PicoHardwareI2C(){
        
    }

    ~PicoHardwareI2C(){
        end();
    }

    virtual void begin() {

    }

    virtual void begin(uint8_t address) {

    }

    virtual void end() {

    }

    virtual void setClock(uint32_t freq) {

    }
  
    virtual void beginTransmission(uint8_t address) {

    }

    virtual uint8_t endTransmission(bool stopBit) {

    }
    
    virtual uint8_t endTransmission(void) {

    }

    virtual size_t requestFrom(uint8_t address, size_t len, bool stopBit) {

    }

    virtual size_t requestFrom(uint8_t address, size_t len) {

    }

    virtual size_t write(uint8_t) {

    }

    virtual int read() {

    }

    virtual int peek() {

    }

    virtual int available() {

    }

    virtual void onReceive(void(*)(int)) {

    }

    virtual void onRequest(void(*)(void)) {

    };
};
