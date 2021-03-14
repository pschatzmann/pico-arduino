
#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

/**
 * @brief The PIO subsystem on RP2040 allows you to write small, simple programs for what are called PIO state machines, of
 * which RP2040 has eight split across two PIO instances. A state machine is responsible for setting and reading one or
 * more GPIOs, buffering data to or from the processor (or RP2040’s ultra-fast DMA subsystem), and notifying the
 * processor, via IRQ or polling, when data or attention is needed
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PIOManager {
    public:
        PIOManager(){
        }
        ~PIOManager(){
            stop();
        }

        PIOManager pin(int pin){
            this->pin = pin;
            return *this;
        }
        PIOManager program(pio_program_t program){
            this->program = program;
            return *this;
        }
        PIOManager config(pio_sm_config config){
            this->config = config;
            return *this;
        }
        PIOManager pio(PIO pio){
            this->pio = pio;
            return *this;
        }

        PIOManager &begin(){
            // to remember this location!
            uint offset = pio_add_program(pio, &program);

            // Find a free state machine on our chosen PIO (erroring if there are
            // none). Configure it to run our program, and start it, using the
            // helper function we included in our .pio file.
            state_machine = pio_claim_unused_sm(pio, true);

            // Map the state machine's OUT pin group to one pin, namely the `pin`
            // parameter to this function.
            sm_config_set_out_pins(&config, pin, 1);

            // Set this pin's GPIO function (connect PIO to the pad)
            pio_gpio_init(pio, pin);

            // Set the pin direction to output at the PIO
            sm_config_set_sideset_pins(&config, pin);

            // Load our configuration, and jump to the start of the program
            pio_sm_init(pio, state_machine, offset, &config);
            return *this;
        }
        
        PIOManager &setup(uint64_t hz, int pin,  pio_program_t program, pio_sm_config config, PIO pio = pio0 ){
            setup(pin, program, config, pio);
            // defines the speed
            setFrequency(hz);
            return *this;
        }

        PIOManager setFrequency(uint64_t hz){
            if (hz>0){
                float div = clock_get_hz(clk_sys) / hz);
                sm_config_set_clkdiv(&config, div);
                pio_sm_set_enabled(pio, state_machine, true);
            } else {
                pio_sm_set_enabled(pio, state_machine, false);
            }
            return *this;
        }

        PIOManager &start() {
            pio_sm_set_enabled(pio, state_machine, true);
            return *this;
        }

        PIOManager &stop() {
            pio_sm_set_enabled(pio, state_machine, false);
            return *this;
        }

    protected:
        PIO pio = pio0;
        pio_sm_config config;
        pio_program_t program;
        uint state_machine; 
        int pin;
};
