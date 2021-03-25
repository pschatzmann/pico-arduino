#pragma once

#include "pico/stdlib.h"
#include "pico/sync.h"

namespace pico_arduino {

/**
 * @brief Pico Semaphore which might be useful if you use the 2 processors (e.g. with the Thread class)
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class Semaphore {
    public:
        /// constructor - calls sem_init
        Semaphore(int maxPermits=1, int initalPermits=1){
            sem_init(&sem, initalPermits,maxPermits);
        }

        /// descructor - calls sem_release
        ~Semaphore(){
            sem_release(&sem);
        }

        /// checks if a semaphore is available
        int available(){
            return sem_available(&sem);
        }

        /// calls sem_reset 
        void reset(int permits=1) {
            sem_reset(&sem, permits);
        }

        /// aquires a semaphore - if no time is indicated it is blocking
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

        /// release the semaphore and makes it availale for other processes
        bool release() {
            return sem_release(&sem);
        }

        /// identical with aquire
        bool up(){
            return aquire();   
        }

        /// identical with release
        bool down() {
            return release();
        }

        /// identical with aquire
        bool wait() {
            return aquire();
        }

        /// identical with release
        bool signal(){
            return release();   
        }

    protected: 
        semaphore_t sem;
};

}