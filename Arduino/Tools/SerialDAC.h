#pragma once
#include "Stream.h"
#include "stdint.h"

/**
 * @brief If your Microcontroller does not provide a dedicated analog output you can use the serial output instead as a poor man's work around.
 * The idea is based on the fact that the output of 0b0 leads to 0 Volt on the signal line and the output of 0b1111111111111111 leads
 * to 3.3V. 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class SerialDAC {
  public:
        /// Constructor
        SerialDAC(Stream &out, uint minSampleValue=0, uint maxSampleValue = 65535){
            out_ptr = &out;
            this->maxSampleValue = maxSampleValue;
            this->minSampleValue = minSampleValue;
        }

        /// convert the sample rate to the necessary baud rate (bit per second (bit/s)) - please set the baud rate of your serial interface to the calculated value
        uint baudRate(uint samplingRate) const {
            // sample rate is samples per second - 1 sample has 16 bits
            return samplingRate * 16;
        }

        /// writes a buffer to the tx line
        void write(uint16_t *inValues, int len) const {
            convert(inValues,inValues,len);
            out_ptr->write((const uint8_t *)inValues,len*sizeof(int16_t));
        }

        /// writes a single value to the tx line
        void write(uint16_t value) const {
            out_ptr->write(convertValue(value));
        }   

  protected:
    Stream *out_ptr;
    long maxSampleValue;
    long minSampleValue;
    const uint16_t values[17] = {
         0b0000000000000000,
         0b0000000000000001,
         0b0000000000000011,
         0b0000000000000111,
         0b0000000000001111,
         0b0000000000011111,
         0b0000000000111111,
         0b0000000001111111,
         0b0000000011111111,
         0b0000000111111111,
         0b0000001111111111,
         0b0000011111111111,
         0b0000111111111111,
         0b0001111111111111,
         0b0011111111111111,
         0b0111111111111111,
         0b1111111111111111
    };

    inline uint map(uint x, uint in_min, uint in_max, uint out_min, uint out_max) const {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void convert(uint16_t *inValues, uint16_t *outValues, int len) const{
        for (int j=0;j<len;j++){
            outValues[j] = convertValue(inValues[j]);
        }
    }

    inline uint16_t convertValue(uint16_t value) const {
        uint8_t idx = map(value, -maxSampleValue, maxSampleValue, 0, 16);
        return values[idx];
    }

};