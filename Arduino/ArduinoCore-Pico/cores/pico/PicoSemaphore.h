#pragma once

#include "pico/stdlib.h"
#include "pico/sync.h"

/**
 * @brief Pico Semaphore which might be useful if you use the 2 processors (e.g. with the Thread class)
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class Semaphore {
    public:
        Semaphore(int maxPermits=1, int initalPermits=1){
            sem_init(&sem, initalPermits,maxPermits);
        }

        ~Semaphore(){
            sem_release(&sem);
        }

        int available(){
            return sem_available(&sem);
        }

        void reset(int permits=1) {
            sem_reset(&sem, permits);
        }

        bool aquire(int64_t timeoutMs=-1){
            bool result = false;
            if (timeoutMs<0){
                sem_acquire_blocking(&sem);
                result = true; 
            } else {
                result = sem_acquire_timeout_ms(&sem, timeoutMs);
            }
            return result;
        }

        bool release() {
            return sem_release(&sem);
        }

        bool up(){
            return aquire();   
        }

        bool down() {
            return release();
        }

        bool wait() {
            return aquire();
        }

        bool signal(){
            return release();   
        }

    protected: 
        semaphore_t sem;
};