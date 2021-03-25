#pragma once

#include "pico/stdlib.h"

namespace pico_arduino {

typedef int64_t(* alarm_callback_t )(alarm_id_t id, void *user_data);
typedef bool(* repeating_timer_callback_t )(repeating_timer_t *rt);

enum TimeUnit {MS,US};

/**
 * @brief Alarm functions for scheduling future execution.
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class TimerAlarm {
    public:
        TimerAlarm(){
            alarm_pool_init_default();
            ap = alarm_pool_get_default();
        }

        ~TimerAlarm(){
            stop();
        }

        /// starts the execution of the callback method after the indicated time
        void start(alarm_callback_t callback,  uint64_t time, TimeUnit unit = MS, void *user_data=nullptr,  bool fire_if_past=true){
            switch(unit){
                case MS:
                    // milliseconds 1/1000 sec
                    alarm_id = alarm_pool_add_alarm_in_ms(ap, time, callback, user_data, fire_if_past);
                    break;
                case US:
                    // microseconds 1/1000000  (10^6) sec
                    alarm_id = alarm_pool_add_alarm_in_us(ap, time, callback, user_data, fire_if_past);
                    break;
            }
        }

        /// stops the execution 
        bool stop(){
            return alarm_pool_cancel_alarm(ap, alarm_id);
        }

    protected:
        alarm_id_t alarm_id=-1;
        alarm_pool_t *ap;

};

/**
 * @brief Repeating Timer functions for simple scheduling of repeated execution.
 * 
 */
class TimerAlarmRepeating {
    public:
        TimerAlarmRepeating(){
            alarm_pool_init_default();
            ap = alarm_pool_get_default();
        }

        ~TimerAlarmRepeating(){
            stop();
        }

        /// starts the repeated exection of the callback methods in the indicate period
        bool start(repeating_timer_callback_t callback, uint64_t time, TimeUnit unit = MS, void *user_data=nullptr){
            bool result = false;
            switch(unit){
                case MS:
                    result = alarm_pool_add_repeating_timer_ms(ap, time, callback, user_data,  &timer);
                    break;
                case US:
                    result = alarm_pool_add_repeating_timer_us(ap, time, callback, user_data, &timer);
                    break;
            }
            return result;
        }

        /// stops the execution
        bool stop(){
            return cancel_repeating_timer(&timer);
        }

    protected:
        alarm_pool_t *ap;
        repeating_timer_t timer;
};

}
