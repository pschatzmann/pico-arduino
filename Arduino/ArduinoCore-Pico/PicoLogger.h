#pragma once

#include "Stream.h"


#ifndef PICO_LOG_LEVEL
#define PICO_LOG_LEVEL Error
#endif

/**
 * @brief A simple Logger that writes messages dependent on the log level
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 */
class PicoLogger {
    public:
        /**
         * @brief Supported log levels. You can change the default log level with the help of the PICO_LOG_LEVEL define.
         * 
         */
        enum LogLevel { 
            Debug,
            Info, 
            Warning, 
            Error
        };

        PicoLogger(){}
        ~PicoLogger(){}

        /// activate the logging
        virtual void begin(Stream& out, LogLevel level=PICO_LOG_LEVEL){
            this->log_stream_ptr = &out;
            this->log_level = level;
        }

        /// checks if the logging is active
        virtual bool isLogging(LogLevel level = Info){
            return log_stream_ptr!=nullptr && level >= log_level;
        }

        /// logs an error
        virtual void error(const char *str, const char* str1=nullptr, const char* str2=nullptr){
            log(Error, str, str1, str2);
        }
            
        /// logs an info message    
        virtual void info(const char *str, const char* str1=nullptr, const char* str2=nullptr){
            log(Info, str, str1, str2);
        }

        /// logs an warning    
        virtual void warning(const char *str, const char* str1=nullptr, const char* str2=nullptr){
            log(Warning, str, str1, str2);
        }

        /// writes an debug message    
        virtual void debug(const char *str, const char* str1=nullptr, const char* str2=nullptr){
            log(Debug, str, str1, str2);
        }

        /// write an message to the log
        virtual void log(LogLevel current_level, const char *str, const char* str1=nullptr, const char* str2=nullptr){
            if (log_stream_ptr!=nullptr){
                if (current_level >= log_level){
                    log_stream_ptr->print((char*)str);
                    if (str1!=nullptr){
                        log_stream_ptr->print(" ");
                        log_stream_ptr->print(str1);
                    }
                    if (str2!=nullptr){
                        log_stream_ptr->print(" ");
                        log_stream_ptr->print(str2);
                    }
                    log_stream_ptr->println();
                    log_stream_ptr->flush();
                }
            }
        }

        const char* toStr(int value){
            static char buffer[10];
            itoa(value,buffer,10);
            return (const char*)buffer;
        }


    protected:
        Stream *log_stream_ptr;
        LogLevel log_level;  


};

extern PicoLogger Logger;   // Support for logging

    

