#pragma once
#include "HardwareI2C.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "PicoLogger.h"
#include "pins_arduino.h"
#include <algorithm>    // std::max

namespace pico_arduino {

/**
 * @brief Arduino I2C implementation using the Pico functionality.
 * In Arduino we can read and write individal characters wheresease in Pico the operations have to be done with 
 * arrays. We therefore create a read and write buffer to cache the operations.
 * 
 * - PicoHardwareI2C Wire(i2c0, 160, GP12, GP13);  
 * - PicoHardwareI2C Wire1(i2c1, 160, GP14, GP15);  
 *  
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PicoHardwareI2C : public HardwareI2C {
    public:
      /// Standard Constructor for PicoHardwareI2C class
      PicoHardwareI2C(i2c_inst_t *i2c, int maxBufferSize, int sdaPin, int sclPin){
          this->i2c = i2c;
          this->maxBufferSize = maxBufferSize;
          sda_pin = sdaPin;
          scl_pin = sclPin;
      }

      /// Destructor for PicoHardwareI2C class
      ~PicoHardwareI2C(){
          end();
      }

      /// Initiate the Wire library and join the I2C bus as a master. This should normally be called only once.
      virtual void begin() {
        begin(-1, -1);
      }

      /// Initiate the Wire library and join the I2C bus as a slave. This should normally be called only once.
      virtual void begin(uint8_t address) {
        begin(address, -1, -1);
      }

      /// Initiate the Wire library and join the I2C bus as a master. This should normally be called only once.
      virtual void begin(int sdaPin, int sclPin) {
        Logger.info("begin");
        setupPins(sdaPin, sclPin);
        i2c_init(i2c, 100000);
        i2c_set_slave_mode(i2c, false, 0);
        is_slave_mode = false;
        setupWriteBuffer();
      }

      /// Initiate the Wire library and join the I2C bus as a slave. This should normally be called only once.
      virtual void begin(uint8_t address, int sdaPin, int sclPin) {
        Logger.printf(PicoLogger::Info, "begin %d", address);
        setupPins(sdaPin,sclPin);
        transmission_address = address;
        i2c_init(i2c, 100000);
        i2c_set_slave_mode(i2c, true, address);
        is_slave_mode = true;
        setupWriteBuffer();
      }

      /// Closes the Wire Library
      virtual void end() {
        Logger.info("end");
        i2c_deinit(i2c);
        if (read_buffer !=nullptr){
          delete[]read_buffer;
          read_buffer = nullptr;
        }
        if (write_buffer !=nullptr){
          delete[]write_buffer;
          write_buffer = nullptr;
        }
      }

      /// This function modifies the clock frequency for I2C communication. I2C slave devices have no minimum working clock frequency, however 100KHz is usually the baseline.
      virtual void setClock(uint32_t baudrate) {
        Logger.info("setClock");
        i2c_set_baudrate(i2c, baudrate);
      }
    
      /// Begin a transmission to the I2C slave device with the given address. Subsequently, queue bytes for transmission with the write() function and transmit them by calling endTransmission().
      virtual void beginTransmission(uint8_t address) {
        Logger.printf(PicoLogger::Info, "beginTransmission %d", address);
        transmission_address=address;
      }

      /// Ends a transmission to a slave device that was begun by beginTransmission() and transmits the bytes that were queued by write(). If true, endTransmission() sends a stop message after transmission, releasing the I2C bus.
      virtual uint8_t endTransmission(bool stopBit) {
        Logger.info("endTransmission", stopBit ? "stop" : "no-stop" );
        uint8_t result = flush(stopBit);
        transmission_address = -1;
        return result;
      }
      
      /// Ends a transmission to a slave device that was begun by beginTransmission() and transmits the bytes that were queued by write().
      virtual uint8_t endTransmission(void) {
        return endTransmission(true);
      }

      /// Writes data from a slave device in response to a request from a master, or queues bytes for transmission from a master to slave device (in-between calls to beginTransmission() and endTransmission()).
      virtual size_t write(uint8_t c) {
        Logger.debug("write");
        size_t result = 0;
        if (write_pos>=maxBufferSize){
          flush(false);
        }
        // we just write to a buffer
        if (write_pos<maxBufferSize){
          result = 1;
          write_buffer[write_pos]=c;
          write_pos++;
        }
        return result;
      }

      /// Writes data from a slave device in response to a request from a master, or queues bytes for transmission from a master to slave device (in-between calls to beginTransmission() and endTransmission()).
      size_t write(const char *buffer, size_t size) {
        Logger.debug("write[]");
        // if we have any data in the buffer flush this first
        flush();
        // and now write out the requested data
        return i2c_write_blocking(i2c, transmission_address,(const uint8_t * ) buffer, size, true);
      }

      /// Used by the master to request bytes from a slave device. The bytes may then be retrieved with the available() and read() functions.
      virtual size_t requestFrom(uint8_t address, size_t len, bool stopBit) {
        Logger.printf(PicoLogger::Info,"requestFrom (%d, %ld, %s)", address,len, stopBit?"stop":"no-stop");
        flush();
        setupReadBuffer(len);
        read_address = address;

        // call requestHandler
        if (this->requestHandler!=nullptr){
            Logger.info("requestHandler");
            (*this->requestHandler)();
        }

        // read the data into the read_buffer
        read_len = i2c_read_blocking(i2c, read_address, read_buffer, len, !stopBit);
        read_pos = 0;
        if (read_len==PICO_ERROR_GENERIC){
          Logger.warning("requestFrom->","PICO_ERROR_GENERIC");
          read_len = 0;
        }
        Logger.printf(PicoLogger::Debug, "requestFrom -> %d",read_len);

        // call recieveHandler
        if (read_len>0 && this->recieveHandler!=nullptr){
            Logger.info("recieveHandler");
            (*this->recieveHandler)(read_len);
        }

        return read_len;
      }

      /// Used by the master to request bytes from a slave device. The bytes may then be retrieved with the available() and read() functions.
      virtual size_t requestFrom(uint8_t address, size_t len) {
        return requestFrom(address, len, false);
      } 

      /// Reads a byte that was transmitted from a slave device to a master after a call to requestFrom() or was transmitted from a master to a slave. read() inherits from the Stream utility class.
      virtual int read() {
        // trigger flush of write buffer
        int result = (read_pos<read_len) ? read_buffer[read_pos++] : -1;
        return result;
      }

      /// Peeks current unread byte that was transmitted from a slave device to a master after a call to requestFrom() or was transmitted from a master to a slave. read() inherits from the Stream utility class.
      virtual int peek() {
        Logger.debug("peek");
        return (read_pos<read_len) ? read_buffer[read_pos] : -1;
      }

      /// Returns the number of bytes available for retrieval with read(). This should be called on a master device after a call to requestFrom() or on a slave inside the onReceive() handler.
      virtual int available() {
        int buffer_available = read_len - read_pos;
        if (buffer_available < 0){
          buffer_available = 0;
          Logger.error("buffer_available is negative - corrected to 0");
        }
        Logger.printf(PicoLogger::Debug, "available: %d", buffer_available);
        return buffer_available;
      }

      /// Registers a function to be called when a slave device receives a transmission from a master.
      virtual void onReceive(void(*recieveHandler)(int)) {
        this->recieveHandler = recieveHandler;
        Logger.info("onReceive");
      }

      /// Register a function to be called when a master requests data from this slave device.
      virtual void onRequest(void(*requestHandler)(void)) {
        this->requestHandler = requestHandler;
        Logger.info("onRequest");
      };

    protected:
      bool is_slave_mode;
      i2c_inst_t *i2c;
      int maxBufferSize;
      // write
      uint8_t transmission_address;
      int write_pos;
      uint8_t *write_buffer;

      // read
      size_t read_len;
      size_t read_pos;
      bool read_stop_bit;
      uint8_t read_address;
      uint8_t *read_buffer;

      // handler
      void(*recieveHandler)(int);
      void(*requestHandler)(void);

      // pins 
      int sda_pin;
      int scl_pin;
      bool pin_is_setup = false;

      void setupPins(int sda, int scl){
        if (!pin_is_setup) {
          if (sda>=0){
            sda_pin = sda;
          }
          if (scl>=0){
            scl_pin = scl;
          }
          gpio_set_function(sda_pin, GPIO_FUNC_I2C);
          gpio_set_function(scl_pin, GPIO_FUNC_I2C);
          gpio_pull_up(sda_pin);
          gpio_pull_up(scl_pin);

        }
      }


      int flush(bool stop=false) {
        bool result = 0; // 4:other error
        if (write_pos>0) {
          Logger.printf(PicoLogger::Debug, "flush address:%d, len:%d, end:%s",transmission_address, write_pos, stop ? "stop": "no-stop" );
          int result = i2c_write_blocking(i2c, transmission_address, write_buffer, write_pos, !stop);
          if (result == write_pos){
            result = 0; // OK
          } else if (result < write_pos){
            result = 1; // 1:data too long to fit in transmit buffer
          } else {
            result = 4; // 4:other error
          }
          write_pos = 0;
          Logger.printf(PicoLogger::Debug, "flush->%d", result);
          
        }

        return result;
      }  

      void  setupWriteBuffer(){
        // setup buffer only if needed
        if (write_buffer==nullptr){
          Logger.info("setupWriteBuffer");
          write_buffer = new uint8_t(maxBufferSize);
        }
      }

      void  setupReadBuffer(int len){
        if (read_buffer==nullptr){
          // setup buffer only if needed
          maxBufferSize = std::max(len, maxBufferSize);
          read_buffer = new uint8_t(maxBufferSize);
        } else {
          if (len>maxBufferSize){
            // grow the read buffer to the requested size
            maxBufferSize = len;
            delete[] read_buffer;
            read_buffer = new uint8_t(maxBufferSize);
          }
        }
      }

};

}


