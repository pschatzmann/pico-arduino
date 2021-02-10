#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "Stream.h"
#include "I2S/i2s_master.h"
#include "I2S/i2s_slave.h"

/**
 * @brief I2S interface for PICO. Limitations: the master can only write data and the slave only read!
 * The data needs to be 2 channels of 16bits.
 * 
 * Please note that if you run a master and slave concurrently, they need to be on different pios because
 * they use a differnt clock rate !
 * 
 * For a general overview of I2S see https://en.wikipedia.org/wiki/I%C2%B2S
 */
class I2S : public Stream {
    public:
        I2S(PIO pio = pio1, uint stateMachineIndex=0 ){
            this->pio = pio;
            this->sm = stateMachineIndex;
        };

        ~I2S(){
            pio_sm_set_enabled(pio, sm, false);
            pio_sm_unclaim(pio, sm);
        }

        // begin the processing sd = data; sck=data+1; ws = data+2
        void begin(uint pin_sd, uint32_t samplingRate, bool master=true){
            this->is_master = master;
            if (is_master){
                beginMaster(pin_sd, pin_sd+1,pin_sd+2, samplingRate );
            } else {
                beginSlave(pin_sd, samplingRate );
            }
        }

        // support for writing of an an individual char
        virtual size_t write(uint8_t c) {
            // flush write buffer if full
            if (txBuffer.size()==txBuffer.capacity()){
                if (!tryFlush()){
                    // if we could not flush, we stall
                    return 0;
                }
            }
            // add char to tx buffer
            txBuffer.push_back(c);
            return 1;
        }    

        // writes a full buffer - return the full requested len or 0 if the buffer was full
        virtual size_t write(const uint8_t *data, int len) {
            size_t result = 0;
            if (is_master){
                if (!pio_sm_is_tx_fifo_full(pio, sm)){
                    flush();
                    // write to pio - we make sure that we write all bytes by blocking
                    int32_t *ptr = (int32_t *)data;
                    for (int j=0;j<len/4;j++){
                        pio_sm_put_blocking(pio, sm, ptr[j]);   
                    }    
                    result = len;
                }
            } else {
                printf("The slave supports only read");
            }
            return result;

        } 

        virtual void flush() {
            tryFlush();
        }

        // support for reading of individual chars
        virtual int read() {
            fillRxBuffer();
            return rxBuffer.read_char();
        }

        // just peek for 1 char
        virtual int peek() {
            fillRxBuffer();
            return rxBuffer.peek();
        }

        // read of multiple chars
        virtual int readBytes(uint8_t *data, int len){
            int idx = 0;
            if (is_master){
                printf("The master supports only write");
            } else {
                // first flush read rxBuffer - only relevant if read() has been called before

                for (int j=0; j<rxBuffer.available();j++){
                    data[idx++] = rxBuffer.read_char();
                }

                // get data from rx fifx
                int64_t *data = (int64_t *) &rxBuffer; 
                for (int j=0;j<len/4;j++){
                    if (pio_sm_is_rx_fifo_empty(pio, sm)){
                        break;
                    }
                    data[idx++] = pio_sm_get(pio, sm); // get next 4 bytes from rx fifo
                }
            }
            return idx*4;  
        }     

        // Determines the number of bytes which are available to read
        virtual int available() {
            int result = 0;
            result += rxBuffer.available();
            result += pio_sm_get_rx_fifo_level(pio, sm) * 4;
            return result;                
        }

    protected:
        PIO pio;
        uint sm;
        bool is_master;
        RingBufferN<BUFFER_SIZE> rxBuffer;
        Vector<uint8_t> txBuffer = Vector<uint8_t>(BUFFER_SIZE);
    

        void beginMaster(uint pin_sd, uint pin_sck, uint pin_ws, uint32_t samplingRate){

            pinMode(pin_sd, OUTPUT);
            pinMode(pin_sck, OUTPUT);
            pinMode(pin_ws, OUTPUT);

            pio_gpio_init(pio, pin_sd);
            pio_gpio_init(pio, pin_sck);
            pio_gpio_init(pio, pin_ws);

            int offset = pio_add_program(pio, &pio_i2s_master_program);
            pio_sm_config c = pio_i2s_master_program_get_default_config(offset);

            // OUT shifts to right, no autopull
            sm_config_set_out_shift(&c, true, false, 32);

            // PIN mappings
            sm_config_set_out_pins(&c, pin_sd, 1);
            sm_config_set_sideset_pins(&c, pin_sck);

            // SM transmits 64 bit per 16 execution cycles.
            float div = (float)clock_get_hz(clk_sys)/ 16 / samplingRate;
            sm_config_set_clkdiv(&c, div);

            pio_sm_init(pio, sm, offset, &c);
            pio_sm_set_enabled(pio, sm, true);
        }


        void beginSlave(uint pin_sd, uint32_t samplingRate){
            pinMode(pin_sd, INPUT);
            pinMode(pin_sd+1, INPUT); //pin_sck
            pinMode(pin_sd+2, INPUT); //pin_ws

            int offset = pio_add_program(pio, &pio_i2s_slave_program);
            pio_sm_config c = pio_i2s_slave_program_get_default_config(offset);

            // OUT shifts to right, with autopull
            sm_config_set_in_shift(&c, true, true, 32);

            // PIN mappings
            sm_config_set_in_pins(&c, pin_sd);

            // this should be fast enough to catch up with the data
            float div = (float)clock_get_hz(clk_sys) / samplingRate;
            sm_config_set_clkdiv(&c, div);

            pio_sm_init(pio, sm, offset, &c);
            pio_sm_set_enabled(pio, sm, true);
        }

        // fills the rx buffer if it is empty from the PIO Fifo
        void fillRxBuffer() {
            if (rxBuffer.available()==0){
                int64_t value;
                uint8_t *ptr = (uint8_t *) &value;
                 
                for(int j=0;j<pio_sm_get_rx_fifo_level(pio, sm);j++){
                    // write 8 bytes to rxBuffer
                    rxBuffer.store_char(ptr[0]);
                    rxBuffer.store_char(ptr[1]);
                    rxBuffer.store_char(ptr[2]);
                    rxBuffer.store_char(ptr[3]);
                    rxBuffer.store_char(ptr[4]);
                    rxBuffer.store_char(ptr[5]);
                    rxBuffer.store_char(ptr[6]);
                    rxBuffer.store_char(ptr[7]);
                }
            }            
        }

        // we flush the tx buffer if it is full 
        virtual bool tryFlush() {
            bool result = true;
            if (txBuffer.size()>0) {
                result = write((uint8_t*)&txBuffer[0], txBuffer.size());
                if (result) {
                    txBuffer.clear();       
                }
            }     
            return result;
        }

};







