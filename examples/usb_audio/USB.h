#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "bsp/board.h"

#include "tusb.h"
#include "AudioTools/AudioTools.h"

#define AUDIO_SAMPLE_RATE 44100

extern AudioSource *tone_generator;
extern void audio_begin(int sampleFrequency);
extern uint32_t blink_interval_ms;
extern "C" const uint16_t tud_audio_desc_lengths[];

/* Blink pattern
* - 250 ms  : device not mounted
* - 1000 ms : device mounted
* - 2500 ms : device is suspended
*/
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};


class USBAudio {
  public:
        virtual void begin(AudioSource &audio_source, int sampleFrequency=AUDIO_SAMPLE_RATE){
            tone_generator = &audio_source;
            audio_begin(sampleFrequency);
            while (true) {
              tud_task(); // tinyusb device task
              led_blinking_task();
            }
          }

  protected:
    //--------------------------------------------------------------------+
    // BLINKING TASK
    //--------------------------------------------------------------------+
    void led_blinking_task(void) {
      static uint32_t start_ms = 0;
      static bool led_state = false;

      // Blink every interval ms
      if ( board_millis() - start_ms < blink_interval_ms) 
        return; // not enough time
      start_ms += blink_interval_ms;

      board_led_write(led_state);
      led_state = 1 - led_state; // toggle
    }    
};
