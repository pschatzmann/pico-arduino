#pragma once

#include "pico/stdlib.h"
#include "pico/multicore.h"

/**
 * @brief Adds support for running code on the second processor core (core1)
 * 
 */
class Thread {
    public:
        Thread(){
            started(false);
        }

        ~Thread(){
            stop();
        }

        // Run code on core 1.
        bool start(void (*callback)()){
            bool result = false;
            if (!started()){
                multicore_launch_core1(callback);
                started(true);
                result = true;
                Logger.info("Thread started on core1");
            } else {
                Logger.info("Thread not started - because one is already active");
            }
            return result;
        }

        // resets the core
        void stop() {
            multicore_reset_core1();
            started(false);   
        }

        // Send core 1 to sleep.
        void sleep() {
            multicore_sleep_core1();
        }

        // check if the core1 has been started and is not stoped
        bool isRunning() {
            return started();
        }

    protected:
        // get or update static started flag
        bool started(int flag=-1){
            static bool started_flag;
            if (flag!=-1)
                started_flag = flag;
            return started_flag;
        }

};

