#pragma once
#include "hardware/gpio.h"

namespace pico_arduino {

#define DDD0 0
#define DDD1 1
#define DDD2 2
#define DDD3 3
#define DDD4 4
#define DDD5 5
#define DDD6 6
#define DDD7 7

// unsigned long clockCyclesPerMicrosecond() {
//     return 0;
// }

class RegisterValue {
    public:
        RegisterValue(int start, int end, bool analog=false){
            this->start = start;
            this->end = end;
            this->is_analog = analog;
       }

        operator uint8_t() const {
            //uint32_t values = (sio_hw->gpio_in << start) & 0x11111111 ;
            return value >> 24;
        }

        uint8_t operator = (uint32_t value) {
            // this->value = value;
            // sio_hw->gpio_set = mask;
            // sio_hw->gpio_clr = mask;
            return value;
        }

        uint8_t operator |= (uint32_t value) {
            // uint32_t values = (sio_hw->gpio_in << start) & 0x11111111 ;

            // gpio_xor_mask(value)
            // this->value |= value;
            return value;
        }

        uint8_t operator ^= (uint8_t value) {
            return this->value;
        }


    protected:
        uint8_t value;
        int start;
        int end;
        bool is_analog;

};

class RegisterStatus {
    public:
        RegisterStatus(int start, int end, bool analog=false){
            this->start = start;
            this->end = end;
            this->is_analog = analog;
        }

        operator uint8_t() const {
            return value;
        }

        uint8_t operator = (uint8_t value) {
            this->value = value;
            return this->value;
        }

        uint8_t operator ^= (uint8_t value) {
            return this->value;
        }


        uint8_t operator |= (uint8_t value) {
            this->value |= value;
            return this->value;
        }
    protected:
        uint8_t value;
        int start;
        int end;
        bool is_analog;

};


// --- PORTD maps to Arduino digital pins 0 to 7

// //The Port D Data Direction Register - read/write
// inline RegisterStatus DDRD(0,7);
// //The Port D Data Register - read/write 
// inline RegisterValue PORTD(0,7);
// // - The Port D Input Pins Register - read only
// inline RegisterValue PIND(0,7);

// //  ---  PORTB maps to Arduino digital pins 8 to 13 The two high bits (6 & 7) map to the crystal pins and are not usable
// //The Port D Data Direction Register - read/write
// inline RegisterStatus DDRB(8,13);
// //The Port D Data Register - read/write 
// inline RegisterValue PORTB(8,13); 
// // - The Port D Input Pins Register - read only
// inline RegisterValue PINB(8,13);


// // ---  PORTC maps to Arduino analog pins 0 to 5. Pins 6 & 7 are only accessible on the Arduino Mini
// //The Port D Data Direction Register - read/write
// inline RegisterStatus DDRC(0,5, true);
// //The Port D Data Register - read/write 
// inline RegisterValue PORTC(0,5, true); 
// // - The Port D Input Pins Register - read only
// inline RegisterValue PINC(0,5, true); 

}