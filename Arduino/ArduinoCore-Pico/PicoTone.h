#pragma once

#include "PicoTimer.h"
#include "Map.h"

// forward declaratioins
extern int64_t stop_tone_callback(alarm_id_t id, void *user_data);


/**
 * @brief We use the TimerAlarmRepeating to generate tones.
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class PicoTone {
    public:
        PicoTone(){
        }

        ~PicoTone(){
            noTone();
        }

        static  bool generate_sound_callback(repeating_timer_t *rt){
            PicoTone *pt = (PicoTone*)rt->user_data;
            pt->state = !pt->state; // toggle state
            digitalWrite(pt->pin, pt->state); 
            return true;
        }

        void tone(uint8_t pinNumber, unsigned int frequency) {
            pin = pinNumber;
            int delay = 1000 / frequency / 2;
            alarm.start(generate_sound_callback, delay, MS, this);
        }

        void noTone() {
            alarm.stop();
        }

        bool operator==(const PicoTone& other){
            return other.pin == this->pin;
        }

        bool operator!=(const PicoTone& other){
            return other.pin != this->pin;
        }

        int pin;
        bool state;

    protected:
        TimerAlarmRepeating alarm;
};


/**
 * @brief ArduinoPicoTone provides static methods for PicoTone
 * 
 */
class ArduinoPicoTone {
    public:
        static int64_t stop_tone_callback(alarm_id_t id, void *user_data){
            // ugliy hack: we receive the pin number in user_data (not a pointer!)
            PicoTone* ptr = (PicoTone*) user_data;
            noTone(ptr->pin);
            return 0;
        }

        // static interface which supports multipe pins concurrently
        static void tone(uint8_t pinNumber, unsigned int frequency, int duration) {
            PicoTone ptone = pinMap().get(pinNumber);
            if (ptone==empyTone()){
                // add entry
                pinMap().put(pinNumber, ptone);
            } 
            ptone.tone(pinNumber, frequency);
            add_alarm_in_ms(duration, stop_tone_callback, &ptone , false);
        }

        // static interface which supports multipe pins
        static void noTone(uint8_t pinNumber) {
            // find the actual PicoTone with the pin number
            PicoTone ptone = pinMap().get(pinNumber);
            if (ptone!=empyTone()){
                ptone.noTone();
            }
        }

        static const PicoTone empyTone() {
            static const PicoTone EMPTY_TONE;
            return EMPTY_TONE;
        }

        // for static interface
        static  Map<int, PicoTone> pinMap(){
            static Map<int, PicoTone> ArduinoPicoTonePinMap(empyTone()); 
            return ArduinoPicoTonePinMap;
        }


};

    
