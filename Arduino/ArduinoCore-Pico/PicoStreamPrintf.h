#pragma once
#include <stdarg.h>
#include <stdio.h>
#include "Stream.h"


#ifndef PRINTF_BUFFER_SIZE
#define PRINTF_BUFFER_SIZE 512
#endif

namespace pico_arduino {

/**
 *   @brief Support for Serial.printf. The maximum printable length is defined by PRINTF_BUFFER_SIZE which is set to 512 by default
 */
class StreamPrintf {  

    public:
        StreamPrintf(Stream *stream){
            stream_ptr = stream;
        }

        virtual int printf(const char* fmt, ...) {
            int len = 0;
            if (stream_ptr!=nullptr){
                char serial_printf_buffer[PRINTF_BUFFER_SIZE] = {0};
                va_list args;
                va_start(args,fmt);
                len = vsnprintf(serial_printf_buffer,PRINTF_BUFFER_SIZE, fmt, args);
                stream_ptr->print(serial_printf_buffer);
                va_end(args);
            }
            return len;
        }
    protected:
         Stream *stream_ptr;
};

}