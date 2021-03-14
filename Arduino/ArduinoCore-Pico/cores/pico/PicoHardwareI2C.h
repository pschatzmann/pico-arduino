#pragma once
#include "HardwareI2C.h"

/**
 * @brief Arduino I2C implementation using the Pico functionality 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PicoHardwareI2C : public HardwareI2C {
    public:
      PicoHardwareI2C(i2c_inst_t *i2c, int maxBufferSize=100, int rxPin, int txPin){
          this->i2c = i2c;
          this->maxBufferSize = maxBufferSize;
          gpio_set_function(rxPin, GPIO_FUNC_I2C);
          gpio_set_function(txPin, GPIO_FUNC_I2C);
          gpio_pull_up(rxPin);
          gpio_pull_up(txPin);
      }

      ~PicoHardwareI2C(){
          end();
      }

      virtual void begin() {
        i2c_init(i2c, 100000);
        i2c_set_slave_mode(i2c, false, 0);
        is_slave_mode = false;
      }

      virtual void begin(uint8_t address) {
        i2c_set_slave_mode(i2c, true, address);
        is_slave_mode = true;
      }

      virtual void end() {
        i2c_deinit(i2c);
      }

      virtual void setClock(uint32_t baudrate) {
        i2c_set_baudrate(i2c, baudrate);
      }
    
      virtual void beginTransmission(uint8_t address) {
        transmission_address=address;
      }

      virtual uint8_t endTransmission(bool stopBit) {
        flush(true);
        transmission_address = -1;
      }
      
      virtual uint8_t endTransmission(void) {
        flush(false);
        transmission_address = -1;
      }

      virtual size_t write(uint8_t c) {
        setupWriteBuffer();
        if (pos>=maxBufferSize){
          flush(false);
        }
        // we just write to a buffer
        write_buffer[pos]=c;
        pos++;
      }

      size_t write(const char *buffer, size_t size) {
        setupWriteBuffer();
        return i2c_write_blocking(i2c, transmission_address, write_buffer, size, true);
      }

      virtual size_t requestFrom(uint8_t address, size_t len, bool stopBit) {
        read_address = address;
        read_len = len;
        read_stop_bit = stopBit;
      }

      virtual size_t requestFrom(uint8_t address, size_t len) {
        requestFrom(address, len, true);
      } 

      virtual int read() {
        readBuffer();
        return (read_pos<read_len) ? buffer[read_pos++] : -1;
      }

      virtual int peek() {
        readBuffer();
        return (read_pos<read_len) ? buffer[read_pos] : -1;
      }

      virtual int available() {
        readBuffer();
        int buffer_available = read_len - read_pos;
        return buffer_available;
      }

      virtual void onReceive(void(*recieveHandler)(int)) {
        this->recieveHandler = recieveHandler;
        Logger.error("onReceive not implemented yet");
      }

      virtual void onRequest(void(*requestHandler)(void)) {
        this->requestHandler = requestHandler;
        Logger.error("onRequest not implemented yet");
      };

    protected:
      i2c_inst_t *i2c;
      int maxBufferSize;
      // write
      uint8_t transmission_address;
      int pos;
      uint8_t *write_buffer;

      // read
      size_t read_len;
      size_t read_pos;
      bool read_stop_bit;
      uint8_t read_address;
      uint8_t *read_buffer;

      // handler
      void(*recieveHandler)(int);
      void(*requestHandler)(void)


      bool flush(bool end) {
        int len = i2c_write_blocking(i2c, transmission_address, write_buffer, pos, !end);
        pos = 0;
      }  

      void  setupWriteBuffer(){
        // setup buffer only if needed
        if (write_buffer==nullptr){
          write_buffer = new uint8_t(maxBufferSize);
        }
      }

      void readBuffer() {
        // allocate buffer only if it is needed
        if (read_buffer==nullptr)
          read_buffer = new uint8_t(maxBufferSize);

        if (read_pos==read_len){
          // fill read buffer
          int len = min(i2c_get_read_available(i2c), maxBufferSize);
          read_len = i2c_read_blocking(i2c, read_address, read_buffer, len, read_stop_bit);
          read_pos = 0;
        }
      }

};
