#pragma once

#include "pico/stdlib.h"
#include "pico/multicore.h"

namespace pico_arduino {

/**
 * @brief The RP2040 contains two FIFOs for passing data, messages or ordered events between the two cores. Each FIFO is 32
 * bits wide, and 8 entries deep. One of the FIFOs can only be written by core 0, and read by core 1. The other can only be
 * written by core 1, and read by core 0.
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class MuliticoreFifo {
    public:
        /**
         * @brief Construct a new Muliticore Fifo object
         * 
         */
        MuliticoreFifo(){
        }

        ~MuliticoreFifo(){
            clear();
        }

        /// Adds an elelment to the end
        bool push(uint32_t data,bool is_blocking=true){
            bool result = true;
            if (!is_blocking){
                result = multicore_fifo_wready();
            }
            if (result){
                multicore_fifo_push_blocking(data);
            } 
            return result;
        }

        /// Retrieves the next element (from the front) and removes it from the collection
        bool pop(uint32_t &data, bool is_blocking=true){
            bool result = true;
            if (!is_blocking){
                result = !isEmpty();
            }
            if (result){
                data = multicore_fifo_pop_blocking();
            }
            return result;
        }

        /// Checks if the fifo is empty
        bool isEmpty() {
            return !multicore_fifo_rvalid();
        }

        /// Checks if the fifo is full
        bool isFull() {
            return !multicore_fifo_wready();
        }

        /// Removes all entries
        void clear(){
            multicore_fifo_drain();
        }
};

}