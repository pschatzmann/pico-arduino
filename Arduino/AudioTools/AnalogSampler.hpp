#pragma once

#include "Stream.h"
#include "AudioTools.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

/**
 * @brief We are sampling an analog pin in the indicated frequency and write the output the the
 * indicated destination stream.
 * 
 */
class PicoAnalogSampler {

    public:
        PicoAnalogSampler(Stream &out, int sampleRate, int pinNumber, int bufferSize=10000){
            this->instance = this;
            this->out = &out;

            this->buffer = new SingleBuffer<uint8_t>(bufferSize);
            dma_chan = dma_claim_unused_channel(true);

            adc_fifo_drain();
            adc_run(false);

            // set sample rate
            int clock_speed_hz =  48000000; // cyles per second
            int sample_cycles = 96;
            int clock_speed_samples_hz = clock_speed_hz / sample_cycles;
            int div = clock_speed_samples_hz / sampleRate; 
            adc_set_clkdiv(div);

            // use DMA to write sampeles to
            cfg = dma_channel_get_default_config(dma_chan);
            dma_channel_configure(dma_chan, &cfg,
                buffer->data(), // dst
                &adc_hw->fifo, // src
                bufferSize, // transfer count
                true // start immediately
            );
        }

        ~PicoAnalogSampler(){
            adc_run(false);
            adc_fifo_drain();
            dma_channel_unclaim (dma_chan);
            delete buffer;
        }

        /**
         * @brief starts the processing on core 1
         * 
         */
        void start() {
            multicore_launch_core1(doLoopBlocking);
        }

        /**
         * @brief Call this method in your loop if you did not start it 
         * 
         */
        void doLoop() {
            dma_channel_wait_for_finish_blocking(dma_chan);
            out->write(buffer->data(), buffer->sizeBytes());
        }

    protected:
        Stream *out;
        uint dma_chan;
        SingleBuffer<uint8_t> *buffer;
        static PicoAnalogSampler *instance;
        dma_channel_config cfg; 

        static void doLoopBlocking(){
            while(true){
                instance->doLoop();
            }
        }
};