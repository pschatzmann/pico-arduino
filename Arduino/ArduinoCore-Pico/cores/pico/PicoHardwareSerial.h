#pragma once

#include "Stream.h"
#include "HardwareSerial.h"
#include "RingBuffer.h"
#include "pico/stdlib.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 512
#endif

/**
 * @brief PicoDefaultSerial is using the pico default output. It is mapped to the Arduino Serial variable.
 * 
 */
class PicoDefaultSerial : public HardwareSerial {
    public:
        PicoDefaultSerial(){
            stdio_init_all();
        }

        using Print::write; // pull in write(str) and write(buf, size) from Print
        using Print::print; // pull in write(str) and write(buf, size) from Print
        using Print::println; // pull in write(str) and write(buf, size) from Print


        virtual void begin(unsigned long baudrate=PICO_DEFAULT_UART_BAUD_RATE) {
            open = true;
        };
        virtual void begin(unsigned long baudrate, uint16_t config){
            open = true;

        }
        virtual void end() {
            open = false;
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
            readBuffer();
            return buffer.read_char();
        }

        virtual void flush(void) {
             stdio_flush();
        }

        virtual size_t write(uint8_t c) {
            return putchar(c);
        }

        virtual int println(char const* str){
            return printf("%s\n",str);
        }

        virtual operator bool(){
            return open;
        };

    protected:
        bool open;
        RingBufferN<BUFFER_SIZE> buffer;

        void readBuffer() {
            if (buffer.available()==0){
                char c = getchar();
                while(c!=EOF && buffer.availableForStore()>0){
                    buffer.store_char(c);
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

        PicoHardwareSerial(int uart_addr, int uart_no) {
            void *ptr = (void*) uart_addr;
            this->uart = (uart_inst_t *)ptr;
            this->uart_no = uart_no;
        }

        virtual void begin(unsigned long baudrate=PICO_DEFAULT_UART_BAUD_RATE) {
            begin(baudrate);
        }

        virtual void begin(unsigned long baudrate, uint16_t config) {
            begin(baudrate, config);
        }

        /**
         * @brief Initialization to output to UART
         * 
         * @param baudrate 
         * @param config 
         * @param rxPin 
         * @param txPin 
         * @param invert 
         * @param cts 
         * @param rts 
         */

        virtual void begin(unsigned long baudrate, uint32_t config, int rxPin=-1, int txPin=-1, bool invert=false, bool cts=false, bool rts=false) {
            rx_pin = rxPin;
            tx_pin = txPin;
            setupDefaultRxTxPins();
            stdio_uart_init_full(uart, baudrate, tx_pin, rx_pin);
            uart_set_hw_flow(uart, cts, rts);
            set_config(config);
        }

        virtual void end(){
             uart_deinit(uart);
        }

        virtual int available(void){
            if (buffer.available()>0){
                return buffer.available();
            }
            return uart_is_readable(uart);
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
             uart_putc(uart, c);
             return 1;
        }

        inline size_t write(const uint8_t *buffer, size_t size) {
            uart_write_blocking(uart, buffer, size);
            return size;
        }

        inline size_t write(const char * buffer, size_t size) {
            return write((uint8_t*) buffer, size);
        }

        inline size_t write(const char * s){
            return write((uint8_t*) s, strlen(s));
        }

        inline size_t write(unsigned long n){
            return write((uint8_t) n);
        }

        inline size_t write(long n){
            return write((uint8_t) n);
        }

        inline size_t write(unsigned int n) {
            return write((uint8_t) n);
        }

        inline size_t write(int n){
            return write((uint8_t) n);
        }

        uint32_t baudRate(){
            return baud_rate;
        }
        
        using Print::write; // pull in write(str) and write(buf, size) from Print
        using Print::print; // pull in write(str) and write(buf, size) from Print
        using Print::println; // pull in write(str) and write(buf, size) from Print

        virtual operator bool() {
            return ok;
        }

        virtual void flush(void) {
        };


    protected:
        RingBufferN<BUFFER_SIZE> buffer;
        uart_inst_t *uart;
        uint baud_rate;
        int tx_pin;
        int rx_pin;
        int uart_no;
        bool ok = false;

        void readBuffer() {
            if (buffer.available()==0){
                char c = uart_getc(uart);
                while(c!=0 && buffer.availableForStore()>0){
                    buffer.store_char(c);
                }
            }
        }

        void set_config(uint32_t config){
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
            // we use different pins for uart0 and uar1. We assign values only if it has not been defined in setup
            if (uart_no==0){
                if (rx_pin==-1) {
                    rx_pin = 2;
                }
                if (tx_pin==-1){
                    tx_pin = 1;
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
                Logger.info("rxPin is ", toStr(rx_pin));
                Logger.info("txPin is ", toStr(tx_pin));
            }
        }

        const char* toStr(int value){
            static char buffer[10];
            itoa(value,buffer,10);
            return (const char*)buffer;
        }
};

// #if !defined(TINYUSB_HOST_LINKED) && !defined(TINYUSB_DEVICE_LINKED)
// /**
//  * @brief Output class which uses USB
//  * 
//  */
// class PicoUSBSerial : public PicoDefaultSerial {
//     PicoUSBSerial(){
//         stdio_usb_init();
//     }

//     // this does not do anything
//     irtual void begin() {
//     }
// };
// #endif