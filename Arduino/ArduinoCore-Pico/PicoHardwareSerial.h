#pragma once

#include "Stream.h"
#include "HardwareSerial.h"
#include "RingBuffer.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <tusb.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 512
#endif

#ifndef READ_WAIT_US 
#define READ_WAIT_US 100
#endif

extern "C" {
static bool tud_cdc_connected (void);
}

/**
 * @brief PicoDefaultSerial is using the pico default output. It is mapped to the Arduino Serial variable.
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PicoDefaultSerial : public HardwareSerial {
    public:
        PicoDefaultSerial(){
        }

        virtual void begin(unsigned long baudrate=PICO_DEFAULT_UART_BAUD_RATE) {
            stdio_init_all();
            is_open = true;
        };
        virtual void begin(unsigned long baudrate, uint16_t config){
            stdio_init_all();
            is_open = true;

        }
        virtual void end() {
            is_open = false;
        }
        virtual int available(void){
            readBuffer();
            return buffer.available();
        }
        virtual int peek(void) {
            readBuffer();
            return buffer.peek();
        }
        virtual int read(void){
            // we might need to flush the current ouptut...
            flush();
            // try to refill the buffer
            readBuffer();
            return buffer.read_char();
        }

        virtual void flush(void) {
             stdio_flush();
        }

        virtual size_t write(uint8_t c) {
            size_t len = putchar(c);
            stdio_flush();
            return len;
        }

        virtual int println(){
            return println("");
        }

        virtual int println(char const* str){
            int len = printf("%s\n",str);
            flush();
            return len;
        }

        using Print::write; // pull in write(str) and write(buf, size) from Print
        using Print::print; // pull in write(str) and write(buf, size) from Print
        using Print::println; // pull in write(str) and write(buf, size) from Print

        virtual operator bool(){
            return tud_cdc_connected() && is_open;
        };

    protected:
        bool is_open;
        RingBufferN<BUFFER_SIZE> buffer;

        void readBuffer() {
            // refill buffer only if it is empty
            if (buffer.available()==0){
                // fill buffer as long as we have space
                int c = getchar_timeout_us(1000);
                while(c!=PICO_ERROR_TIMEOUT && buffer.availableForStore()>0){
                    buffer.store_char(c);
                    c = getchar_timeout_us(1000);
                }
            }
        }


};

/**
 * @brief Serial Stream for a defined UART. By default we use the following pins: UART0 tx/rx = gp0/gp1; UART1 tx/rx = gp4/gp5; 
 * 
 */
class PicoHardwareSerial : public HardwareSerial {
    public:
        PicoHardwareSerial(){
        }

        PicoHardwareSerial(int uart_no) {
            this->uart_no = uart_no;
            this->uart = uart_no == 0 ? uart0 : uart1;
        }

        virtual void begin(unsigned long baudrate=PICO_DEFAULT_UART_BAUD_RATE) {
            begin(baudrate, -1, -1, SERIAL_8N1);
        }

        virtual void begin(unsigned long baudrate, uint16_t config) {
            begin(baudrate, -1, -1, config);
        }

        /**
         * @brief Initialization to output to UART
         * 
         * @param baudrate 
         * @param rxPin 
         * @param txPin 
         * @param config 
         * @param invert 
         * @param cts 
         * @param rts 
         */

        virtual void begin(unsigned long baudrate, int rxPin, int txPin, uint32_t config=SERIAL_8N1,  bool invert=false, bool cts=false, bool rts=false) {
            Logger.info("begin", toStr(baudrate));
            rx_pin = rxPin;
            tx_pin = txPin;
            uart_init(uart, baudrate);
            setupDefaultRxTxPins();
            uart_set_hw_flow(uart, cts, rts);
            set_config(config);
            uart_set_translate_crlf(uart, false);
            uart_set_fifo_enabled(uart, true);

            uint rate_effective = uart_set_baudrate(uart,baudrate);
            if (Logger.isLogging(PicoLogger::Info)) {
                Logger.info("baud_rate requested:",toStr(baudrate));
                Logger.info("baud_rate effective:",toStr(rate_effective));
            }
            open = true;
        }

        virtual void end(){
             uart_deinit(uart);
             open = false;
        }

        virtual int available(void){
            readBuffer();
            return buffer.available();
        }

        virtual int availableForWrite(void){
            return uart_is_writable(uart);
        }

        virtual int peek(void){
            readBuffer();
            return buffer.peek();
        }

        virtual int read(void){
            readBuffer();
            return buffer.read_char();
        }

        virtual size_t write(uint8_t c) {
             bool ok = uart_is_writable (uart);
             if (ok){
                uart_putc(uart, c);
             }
             return ok ? 1 : 0;
        }

        inline size_t write(const uint8_t *buffer, size_t size) {
            uart_write_blocking(uart, buffer, size);
            return size;
        }

        inline size_t write(const char * buffer, size_t size) {
            return write((uint8_t*) buffer, size);
        }

        inline size_t print(const char * buffer) {
            return write((uint8_t*) buffer, strlen(buffer));
        }

        inline size_t println(const char * buffer) {
            size_t result = print(buffer);
            result += write((uint8_t*)"\r\n", 2);
            return result;
        }

        inline size_t write(const char * s){
            return write((uint8_t*) s, strlen(s));
        }

        uint32_t baudRate(){
            return baud_rate;
        }
        
        using Print::write; // pull in write(str) and write(buf, size) from Print
        using Print::print; // pull in write(str) and write(buf, size) from Print
        using Print::println; // pull in write(str) and write(buf, size) from Print


        virtual void flush(void) {
        };

        virtual operator bool(){
            return open;
        };


    protected:
        RingBufferN<BUFFER_SIZE> buffer;
        uart_inst_t *uart;
        uint baud_rate;
        int tx_pin;
        int rx_pin;
        int uart_no;
        bool open = false;

        // filles the read buffer
        void readBuffer(bool refill=false) {
            // refill buffer only when requested or when it is empty
            if (refill || buffer.available()==0){
                if (uart_is_readable_within_us(uart, READ_WAIT_US)) {
                    while(buffer.availableForStore()>0 && uart_is_readable(uart) ) {
                        char c = uart_getc(uart);
                        buffer.store_char(c);
                    }
                }
            }
        }

        void set_config(uint32_t config){
            Logger.info("set_config");
            //data, parity, and stop bits
            switch(config){
            case SERIAL_5N1:
                uart_set_format(uart, 5, 1,UART_PARITY_NONE);
                break;
            case SERIAL_6N1:
                uart_set_format(uart, 6, 1,UART_PARITY_NONE);
                break;
            case SERIAL_7N1:
                uart_set_format(uart, 7, 1,UART_PARITY_NONE);
                break;
            case SERIAL_8N1:
                Logger.info("SERIAL_8N1 - UART_PARITY_NONE");
                uart_set_format(uart, 8, 1,UART_PARITY_NONE);
                break;
            case SERIAL_5N2:
                uart_set_format(uart, 5, 2,UART_PARITY_NONE);
                break;
            case SERIAL_6N2:
                uart_set_format(uart, 6, 2,UART_PARITY_NONE);
                break;
            case SERIAL_7N2:
                uart_set_format(uart, 7, 2,UART_PARITY_NONE);
                break;
            case SERIAL_8N2:
                uart_set_format(uart, 8, 2,UART_PARITY_NONE);
                break;
            case SERIAL_5E1: 
                uart_set_format(uart, 5, 1,UART_PARITY_EVEN);
                break;
            case SERIAL_6E1:
                uart_set_format(uart, 6, 1,UART_PARITY_EVEN);
                break;
            case SERIAL_7E1:
                uart_set_format(uart, 7, 1,UART_PARITY_EVEN);
                break;
            case SERIAL_8E1:
                uart_set_format(uart, 8, 1,UART_PARITY_EVEN);
                break;
            case SERIAL_5E2:
                uart_set_format(uart, 5, 2,UART_PARITY_EVEN);
                break;
            case SERIAL_6E2:
                uart_set_format(uart, 6, 2,UART_PARITY_EVEN);
                break;
            case SERIAL_7E2:
                uart_set_format(uart, 7, 2,UART_PARITY_EVEN);
                break;
            case SERIAL_8E2:
                uart_set_format(uart, 8, 2,UART_PARITY_EVEN);
                break;
            case SERIAL_5O1: 
                uart_set_format(uart, 5, 1,UART_PARITY_ODD);
                break;
            case SERIAL_6O1:
                uart_set_format(uart, 6, 1,UART_PARITY_ODD);
                break;
            case SERIAL_7O1:
                uart_set_format(uart, 7, 1,UART_PARITY_ODD);
                break;
            case SERIAL_8O1:
                uart_set_format(uart, 8, 1,UART_PARITY_ODD);
                break;
            case SERIAL_5O2:
                uart_set_format(uart, 5, 2,UART_PARITY_ODD);
                break;
            case SERIAL_6O2:
                uart_set_format(uart, 6, 2,UART_PARITY_ODD);
                break;
            case SERIAL_7O2:
                uart_set_format(uart, 7, 2,UART_PARITY_ODD);
                break;
            case SERIAL_8O2:
                uart_set_format(uart, 8, 2,UART_PARITY_ODD);
                break;
            };
        }

        void setupDefaultRxTxPins(){
            Logger.info("setupDefaultRxTxPins");
            // we use different pins for uart0 and uar1. We assign values only if it has not been defined in setup
            if (uart_no==0){
                if (rx_pin==-1) {
                    rx_pin = 1;
                }
                if (tx_pin==-1){
                    tx_pin = 0;
                }
            } else {
                if (rx_pin==-1){
                    rx_pin = 5;
                }
                if (tx_pin==-1){
                    tx_pin = 4;
                }
            }
            // display pin assignments
            if (Logger.isLogging()) {
                Logger.info("Using UART: ", toStr(uart_no));
                Logger.info("txPin is ", toStr(tx_pin));
                Logger.info("rxPin is ", toStr(rx_pin));
            }
            if (tx_pin!=-1) {
                gpio_set_function(tx_pin, GPIO_FUNC_UART);
            }
            if (rx_pin!=-1){
                gpio_set_function(rx_pin, GPIO_FUNC_UART);
            }

        }

        const char* toStr(int value){
            static char buffer[10];
            itoa(value,buffer,10);
            return (const char*)buffer;
        }
};

