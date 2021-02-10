#pragma once

#include "pico/stdlib.h"
#include "pico/multicore.h"

/**
 * @brief Adds support for running code on the second processor core (core1)
 * 
 */

typedef void (*ThreadCB)(void* arg);


class Thread {
    public:
        Thread(){
            started(false);
        }

        ~Thread(){
            stop();
        }

        // Run code on core 1 - we can pass a reference to an object which will be given to the callback as argument
        bool start(ThreadCB callback, void* ptr=nullptr){
            bool result = false;
            // set arguments to callback
            staticPtr(ptr);
            staticCallback(callback);

            if (!started()){
                multicore_launch_core1(callback_handler);
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

        static void callback_handler(){
            ThreadCB cb = staticCallback();
            void* ptr = staticPtr();
            // calling callback
            cb(ptr);
        }

    protected:
        // static setter / getter for ptr
        static void *staticPtr(void* ptr = nullptr){
            static void *stat_ptr = {ptr};
            return stat_ptr;
        }

        // static setter / getter for callback
        static ThreadCB staticCallback(ThreadCB cb = nullptr) {
            static ThreadCB stat_cb = {cb};
            return stat_cb;
        };


        // get or update static started flag
        bool started(int flag=-1){
            static bool started_flag;
            if (flag!=-1)
                started_flag = flag;
            return started_flag;
        }

};

