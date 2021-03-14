#pragma once

#include "HardwareSPI.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"

/**
 * @brief Arduino HardwareSPI interface using the Pico API. 
 * We use the following default pins
 * spi0:  pinRx = 16; pinTx = 19; pinCS = 17; pinSCK = 18;
 * spi1:  pinRx = 12; pinTx = 11; pinCS = 13; pinSCK = 10;
 * On the master miso = pinRx and mosi = pinTx
 * 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PicoHardwareSPI : public HardwareSPI {
    public:
        PicoHardwareSPI(spi_inst_t *spi){
            this->spi = spi;
        }

        ~PicoHardwareSPI(){
            end();
        }

        /**
         * @brief Initializes the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high.
         * 
         */
        virtual void begin() {
            begin(false, -1,-1,-1,-1);
        };

        /**
         * @brief Initializes the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high.
         * 
         * @param slave 
         * @param pinRx 
         * @param pinTx 
         * @param pinCS 
         * @param pinSCK 
         */
        virtual void begin(bool slave, int pinRx=-1, int pinTx=-1, int pinCS=-1, int pinSCK=-1) {            
            Logger.info("begin");
            is_slave = slave;
            setupPins(pinRx, pinTx, pinCS, pinSCK);
        }
        
        /**
         * @brief Disables the SPI bus (leaving pin modes unchanged).
         * 
         */
        virtual void end() {
            Logger.info("end");
            spi_deinit(spi);
            is_init = false;
        }

        /**
         * @brief Initializes the SPI bus using the defined SPISettings.
         * 
         * @param settings 
         */
        virtual void beginTransaction(SPISettings settings) {
            Logger.debug("beginTransaction");
            is_transaction = true;

            // initialize on first transction
            if (!is_init){
                Logger.info("spi_init");
                spi_init (spi, settings.getClockFreq() );

                // Configure the SPI for master- or slave-mode operation. By default, spi_init() sets master-mode.
                if (is_slave){
                    spi_set_slave(spi, is_slave);
                }

                is_init = true;
            }

            // set baud rate and format on change of settings
            if (last_settings != settings){
                SPIMode mode = settings.getDataMode();
                Logger.info("spi_set_baudrate", Logger.toStr(settings.getClockFreq()));
                spi_set_baudrate(spi, settings.getClockFreq());

                switch (mode) {
                    case SPI_MODE0:
                        Logger.info("mode", "MODE0");                     
                        cpol=SPI_CPOL_0;cpha=SPI_CPHA_0;
                        break;
                    case SPI_MODE1:
                        Logger.info("mode", "MODE0");                     
                        cpol=SPI_CPOL_0;cpha=SPI_CPHA_1;
                        break;
                    case SPI_MODE2:
                        Logger.info("mode", "MODE0");                     
                        cpol=SPI_CPOL_1;cpha=SPI_CPHA_0;
                        break;
                    case SPI_MODE3:
                        Logger.info("mode", "MODE0");                     
                        cpol=SPI_CPOL_1;cpha=SPI_CPHA_1;
                        break;
                    default:
                        Logger.error("Invalid Mode");                     
                        break;

                }

                BitOrder order_arduino = settings.getBitOrder();
                order = (order_arduino == LSBFIRST ) ?  SPI_LSB_FIRST : SPI_MSB_FIRST;
                data_bits = 0;
                last_settings = settings;
            }

            if (using_interrupt_no!=0) {
                Logger.info("irq_set_enabled false");
                irq_set_enabled(using_interrupt_no, false);
            }
        }

        /**
         * @brief Stop using the SPI bus. Normally this is called after de-asserting the chip select, to allow other libraries to use the SPI bus.
         * 
         */
        virtual void endTransaction(void) {
            Logger.debug("endTransaction");
            is_transaction = false;

            if (using_interrupt_no!=0) {
                Logger.info("irq_set_enabled true");
                irq_set_enabled(using_interrupt_no, true);
            }
        }
       
        /**
         * @brief SPI transfer is based on a simultaneous send and receive of 1 byte
         * 
         * @param data sent 
         * @return uint8_t data received
         */
        virtual uint8_t transfer(uint8_t data) {
            Logger.debug("transfer");
            setDataWidth(8);
            uint8_t array[1]={data};
            uint8_t arrayResult[1] = {0};
            spi_write_read_blocking(spi,  array, arrayResult, 1);
            return arrayResult[0];          
        }

        /**
         * @brief SPI transfer is based on a simultaneous send and receive of 2 bytes
         * 
         * @param data sent 
         * @return uint8_t data received
         */
        virtual uint16_t transfer16(uint16_t data){
            Logger.debug("transfer16");
            setDataWidth(16);
            uint16_t result;
            spi_write16_read16_blocking(spi, &data, &result, 1);
            return result;
        }
    
        /**
         * @brief SPI transfer is based on a simultaneous send and receive of len bytes
         * 
         * @param array data sent and received
         * @param len length of data in bytes
         */
    
        virtual void transfer(void *array, size_t len) {
            Logger.debug("transfer (array)");
            setDataWidth(8);
            spi_write_read_blocking(spi,  (const uint8_t*) array, (uint8_t*) array, len);
        }
        
        /**
         * @brief If your program will perform SPI transactions within an interrupt, call this function to register the interrupt number or name with the SPI library. 
         * This allows SPI.beginTransaction() to prevent usage conflicts. Note that the interrupt specified in the call to usingInterrupt() will be disabled on a call 
         * to beginTransaction() and re-enabled in endTransaction().
         * 
         * @param interruptNumber 
         */
        virtual void usingInterrupt(int interruptNumber) {
            Logger.info("usingInterrupt", Logger.toStr(interruptNumber));
            using_interrupt_no = interruptNumber;
        }
        
        /// If your program will perform SPI transactions within an interrupt, call this function to de-register the interrupt number 
        virtual void notUsingInterrupt(int interruptNumber) {
            Logger.info("notUsingInterrupt",Logger.toStr(interruptNumber));
            irq_set_enabled(interruptNumber, true);
            using_interrupt_no = 0;
        }
    
        /// Enable the SPI interrupt: SPI0_IRQ = 18, SPI1_IRQ = 19
        virtual void attachInterrupt() {
            Logger.info("attachInterrupt");
            int interrupt = getStandardInterrupt();
            if(interrupt>0){
                irq_set_enabled(interrupt, true);
            }
        }
        
        /// Turns off the given interrupt: SPI0_IRQ = 18, SPI1_IRQ = 19
        virtual void detachInterrupt() {
            Logger.info("detachInterrupt");
            int interrupt = getStandardInterrupt();
            if(interrupt>0){
                irq_set_enabled(interrupt, false);
            }
        }
        
    protected:
        SPISettings last_settings; // Arduino SPI settings
        spi_inst_t* spi;
        spi_cpol_t cpol;
        spi_cpha_t cpha;
        spi_order_t order;
        uint data_bits;
        int using_interrupt_no;
        bool is_init = false;
        bool is_slave;
        bool is_transaction;

        // SPI0_IRQ = 18, SPI1_IRQ = 19
        int getStandardInterrupt(){
            int interrupt;
            if (spi == spi0){
                interrupt = 18;
            } else if (spi == spi0){
                interrupt = 19;
            }
            return interrupt;
        }

        // adjust data bits
        void setDataWidth(int bits){
            if (data_bits != bits){
                data_bits = bits;
                setFormat();
            }
        }

        void setFormat(){
            Logger.info("spi_set_format");
            spi_set_format(spi, data_bits, cpol, cpha, order);
        }

        void setupPins(int pinRx=-1, int pinTx=-1, int pinCS=-1, int pinSCK=-1){
            if (spi == spi0){
                Logger.info("setupPins for spi0");
                if (pinRx==-1){
                    pinRx = 16;
                }
                if (pinTx==-1){
                    pinTx = 19;
                }
                if (pinCS==-1){
                    pinCS = 17;
                }
                if (pinSCK==-1){
                    pinSCK = 18;
                }
            } else if (spi == spi1){
                Logger.info("setupPins for spi1");
                if (pinRx==-1){
                    pinRx = 12;
                }
                if (pinTx==-1){
                    pinTx = 11;
                }
                if (pinCS==-1){
                    pinCS = 13;
                }
                if (pinSCK==-1){
                    pinSCK = 10;
                }

            } else {
                Logger.error("Invalid SPI device");
            }

            gpio_set_function(pinRx, GPIO_FUNC_SPI);
            gpio_set_function(pinSCK, GPIO_FUNC_SPI);
            gpio_set_function(pinTx, GPIO_FUNC_SPI);

            // Chip select is active-low, so we'll initialise it to a driven-high state
            gpio_init(pinCS);
            gpio_set_dir(pinCS, GPIO_OUT);
            gpio_put(pinCS, 1);

            // display pin assignments
            if (Logger.isLogging()) {
                Logger.info("pinRx is ", Logger.toStr(pinRx));
                Logger.info("pinTx is ", Logger.toStr(pinTx));
                Logger.info("pinSCK is ", Logger.toStr(pinSCK));
                Logger.info("pinCS is ", Logger.toStr(pinCS));
            }

        }
        

    
};