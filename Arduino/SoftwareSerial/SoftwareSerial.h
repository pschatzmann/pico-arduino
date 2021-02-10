#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "SoftwareSerial/pio_uart_tx.h"
#include "SoftwareSerial/pio_uart_rx.h"
/**
 * @brief Software Serial Arduino Stream which uses the Pico PIO.
 * 
 * Based on https://github.com/raspberrypi/pico-examples/tree/master/pio
 */

class SoftwareSerial : public Stream {
    public:
        SoftwareSerial(PIO pio = pio1, uint stateMachineRxIndex=0,uint stateMachineTxIndex=1) {
            this->pio = pio;
            this->sm_rx = stateMachineRxIndex;
            this->sm_tx = stateMachineTxIndex;
        }

        void begin(uint baud=9800, int txPin=-1, int rxPin=-1){
            this->baud = baud;

            if (rxPin >=0 ){
                setupRx(rxPin);
            }

            if (txPin >=0 ){
                setupTx(txPin);
            }
        }

        virtual int peek() {
            peekValue = read();
            return peekValue;
        }

        virtual int available() {
            return pio_sm_get_rx_fifo_level (pio, sm_rx);
        }

        virtual int read() {
            if (peekValue!=-1){
                int result = peekValue;
                peekValue = -1;
                return result;
            }
            // 8-bit read from the uppermost byte of the FIFO, as data is left-justified
            io_rw_8 *rxfifo_shift = (io_rw_8*)&pio->rxf[sm_rx] + 3;
            if (pio_sm_is_rx_fifo_empty(pio, sm_rx))
                return -1;
            
            tight_loop_contents();
            return (char)*rxfifo_shift;
        }    

        virtual size_t write(uint8_t c) {
            pio_sm_put_blocking(pio, sm_tx, (uint32_t)c);
            return 1;
        }


    protected:
        PIO pio;
        uint sm_rx;
        uint sm_tx;
        uint baud;
        int offset;
        int peekValue=-1;

        void setupRx(uint pin) {
            pio_sm_set_consecutive_pindirs(pio, sm_rx, pin, 1, false);
            pio_gpio_init(pio, pin);
            gpio_pull_up(pin);

            int offset = pio_add_program(pio, &pio_uart_rx_program);
            pio_sm_config c = pio_uart_rx_program_get_default_config(offset);
            sm_config_set_in_pins(&c, pin); // for WAIT, IN
            sm_config_set_jmp_pin(&c, pin); // for JMP
            // Shift to right, autopull disabled
            sm_config_set_in_shift(&c, true, false, 32);
            // Deeper FIFO as we're not doing any TX
            sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
            // SM transmits 1 bit per 8 execution cycles.
            float div = (float)clock_get_hz(clk_sys) / (8 * baud);
            sm_config_set_clkdiv(&c, div);
            
            pio_sm_init(pio, sm_rx, offset, &c);
            pio_sm_set_enabled(pio, sm_rx, true);
        }

        void setupTx(uint pin_tx) {
            // Tell PIO to initially drive output-high on the selected pin, then map PIO
            // onto that pin with the IO muxes.
            pio_sm_set_pins_with_mask(pio, sm_tx, 1u << pin_tx, 1u << pin_tx);
            pio_sm_set_pindirs_with_mask(pio, sm_tx, 1u << pin_tx, 1u << pin_tx);
            pio_gpio_init(pio, pin_tx);

            int offset = pio_add_program(pio, &pio_uart_tx_program);
            pio_sm_config c = pio_uart_tx_program_get_default_config(offset);

            // OUT shifts to right, no autopull
            sm_config_set_out_shift(&c, true, false, 32);

            // We are mapping both OUT and side-set to the same pin, because sometimes
            // we need to assert user data onto the pin (with OUT) and sometimes
            // assert constant values (start/stop bit)
            sm_config_set_out_pins(&c, pin_tx, 1);
            sm_config_set_sideset_pins(&c, pin_tx);

            // We only need TX, so get an 8-deep FIFO!
            sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

            // SM transmits 1 bit per 8 execution cycles.
            float div = (float)clock_get_hz(clk_sys) / (8 * baud);
            sm_config_set_clkdiv(&c, div);

            pio_sm_init(pio, sm_tx, offset, &c);
            pio_sm_set_enabled(pio, sm_tx, true);
        }
};