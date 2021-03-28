#pragma once

#include "pico/stdlib.h"
extern "C" {
#include "pico/util/queue.h"
}

namespace pico_arduino {

/**
 * @brief Construct a new Pico Queue object
 * @tparam T 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
template <class T> 
class Queue {
    public:
        /**
         * @brief Construct a new Queue object
         * 
         * @param maxCount max number of entries
         * @param blocking true if blocking
         * @param withLock true if access is synchronized with locks
         */
        Queue( int maxCount = 100,bool blocking=true, bool withLock=true ){
            is_blocking = blocking;
            if (withLock){
                queue_init_with_spinlock(&q,sizeof(T), maxCount, nextSpinlockNumber());
            } else {
                queue_init(&q,sizeof(T),maxCount);
            }
        }

        /// Destructor - calls queue_free
        ~Queue() {
            queue_free(&q);
        }

        /// checks if the queue is eimpty
        bool isEmpty() {
            return queue_is_empty(&q);
        }

        /// checks if the queue is full
        bool isFull() {
            return queue_is_full(&q);
        }

        /// Reads the next element w/o removing it from the queue
        bool peek(T &data){
            bool result = false;
            if (is_blocking){
                queue_peek_blocking(&q, (void*) &data);
                result = true;
            } else {
                result = queue_try_peek(&q, (void*) &data);
            }
            return result;
        }

        /// Adds an element (at the end) of the queue
        bool push(T &data){
            bool result = false;
            if (is_blocking){
                queue_add_blocking(&q, (void*) &data);
                result = true;
            } else {
                result = queue_try_add(&q, (void*) &data);
            }
            return result;
        }

        /// Gets the next element (from the head) and removes it from the queue
        bool pop(T &data){
            bool result = false;
            if (is_blocking){
                queue_remove_blocking(&q, (void*) &data);
                result = true;
            } else {
                result = queue_try_remove(&q, (void*) &data);
            }
            return result;
        }

        /// Provides the number of entries in the queue
        uint size() {
            return queue_get_level(&q);
        }

        /// clears the queue by removing all elements
        void clear() {
            T data;
            while(remove((void*) &data));
        }

    protected:
        queue_t q;
        bool is_blocking;

        uint nextSpinlockNumber(){
            static uint spinlock_num;
            return spinlock_num++;
        }
};

}