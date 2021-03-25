#pragma once
#include "hardware/dma.h"


/**
 * @brief We can use the Pico DMA to copy data "in the background" while the processor is doing some other work. One PicoDMA object represents
 * a single DMA channel
 */
template <class T> 
class PicoDMA {
    public:

        /// Releases the DMA channel and makes it available again
        void releaseChannel(){
            if (channel_no!=-1) dma_channel_unclaim(channel_no);
            channel_no = -1;
        }

        /// provides the used DMA channel - returns -1 if no channel has been set up so far
        int channel() {
            return channel_no;
        }

        /**
         * @brief Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination.
         * The copying is done in the background
         * @param dst copy destination
         * @param src copy source
         * @param len number of entries
         * @param start if true the copy starts immediatly - if set to thrue you need to call start()
         * @return dma channel 
         */
        void copy(T* dst, T* src, int len, boolean start = true){
            setupChannel();
            // we dont use any interrupts in this method
            irq_set_enabled(DMA_IRQ_0, true);

            // 8 bit transfers. Both read and write address increment after each
            // transfer (each pointing to a location in src or dst respectively).
            // No DREQ is selected, so the DMA transfers as fast as it can.
            dma_channel_config c = dma_channel_get_default_config(channel_no);
            channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
            channel_config_set_read_increment(&c, true);
            channel_config_set_write_increment(&c, true);
            dma_channel_configure(
                channel_no,          // Channel to be configured
                &c,            // The configuration we just created
                dst,           // The initial write address
                src,           // The initial read address
                len * sizeof(T) , // Number of transfers; in this case each is 1 byte.
                start           // Start immediately.
            );
        }

        /**
         * @brief Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination.
         * The copying is done in the background
         * @param to copy destination
         * @param from copy source
         * @param len number of entries
         * @param completion_handler interrupt handler which is executed when the copy terminates
         * @return dma channel 
         */
        void copy(T* dst, T* src, int len, void (*completion_handler)(), bool startCopy=true){
            copy(dst, src, len, false);

            // Tell the DMA to raise IRQ line 0 when the channel finishes a block
            dma_channel_set_irq0_enabled(channel_no, true);

            // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
            irq_set_exclusive_handler(DMA_IRQ_0, completion_handler);
            irq_set_enabled(DMA_IRQ_0, true);

            // starts the processing
            if (startCopy) start(channel_no);
        }


        /**
         * @brief Sets a destination array with a fixed value
         * We support only data of the size of uint_8, uint16_t and uint_32_t
         * @param dst destination
         * @param src value
         * @param len length of destination array
         * @param start if true we start immediatly
         * @return dma channel 
         */

        bool set(T* dst, T src, int len, boolean start = true){
            if (sizeof(T)>4){
                Logger.error("Only data up to 32 bytes is supported by DMA");
                return false;
            }
            setupChannel();
            T src_copy = src;
    
            // we dont use any interrupts in this method
            irq_set_enabled(DMA_IRQ_0, true);

            // 8 bit transfers. Both read and write address increment after each
            // transfer (each pointing to a location in src or dst respectively).
            // No DREQ is selected, so the DMA transfers as fast as it can.
            dma_channel_config c = dma_channel_get_default_config(channel_no);
            channel_config_set_transfer_data_size(&c, transferSize());
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, true);
            dma_channel_configure(
                channel_no,          // Channel to be configured
                &c,            // The configuration we just created
                dst,           // The initial write address
                &src_copy,           // The initial read address
                len, // Number of transfers; in this case each is 1 byte.
                start           // Start immediately.
            );
            return true;

        }

        /**
         * @brief Sets a destination array with a fixed value
         * @param to copy destination array
         * @param from  source value
         * @param len length of array 
         * @param completion_handler interrupt handler which is executed when the copy terminates
         * @return dma channel 
         */
        void set(T* dst, T src, int len, void (*completion_handler)(), bool startCopy=true){
            // set values
            set(dst, src, len, false);

            // Tell the DMA to raise IRQ line 0 when the channel finishes a block
            dma_channel_set_irq0_enabled(channel_no, true);

            // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
            irq_set_exclusive_handler(DMA_IRQ_0, completion_handler);
            irq_set_enabled(DMA_IRQ_0, true);

            // starts the processing
            if (startCopy) start(channel_no);
;
        }

        /// starts the copy
        void start(){
            if (channel_no!=-1) dma_channel_start(channel_no);
        }

        /// aborts the copy
        void abort(){
            if (channel_no!=-1) dma_channel_abort(channel_no);
        }

        /// a blocking wait for the copy to complete
        void wait(){
            if (channel_no!=-1) dma_channel_wait_for_finish_blocking(channel_no);
        }

        /// checks if the copy is still in process
        bool isBusy(){
            return channel_no==-1 ? true : dma_channel_is_busy(channel_no);
        } 

        /// You need to call this method in the completion handler
        void clearInterrupt() {
            dma_hw->ints0 = 1u << channel;
            //irq_clear(DMA_IRQ_0);
        }

    protected:
        int channel_no = -1;

        dma_channel_transfer_size transferSize() {
            switch(sizeof(T)){
                case 1:
                    return DMA_SIZE_8;
                case 2:
                    return DMA_SIZE_16;
                case 4:
                    return DMA_SIZE_32;
            }
        }

        /// Provides the next available DMA channel
        void setupChannel() {
            if (channel_no == -1){
                channel_no = dma_claim_unused_channel(true);
            }
        }

};