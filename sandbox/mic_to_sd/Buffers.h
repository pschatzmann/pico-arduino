#pragma once
#include "PicoQueue.h"

/**
 * @brief We are managing a 2 collections of memory arrays: One for the available buffers which can be requested
 * to record data and the buffers of filled data which need to be stored (e.g. to a SD drive) 
 */

template <class T> 
class Buffers {
  public:
    Buffers(int buffer_count, int buffer_length){
        this->buffer_count = buffer_count;
        this->buffer_length = buffer_length;

        emptyBuffers = new Queue<T*>(buffer_count, true, true);
        filledBuffers = new Queue<T*>(buffer_count, true, true);

        // allocate buffers
        for (int j=0;j<buffer_count;j++) {
            T* new_buffer = new T[buffer_length];
            emptyBuffers->push(new_buffer);
        }
    }
     ~Buffers(){
         if (emptyBuffers!=nullptr) delete emptyBuffers;
         if (filledBuffers!=nullptr) delete filledBuffers;
     }

    virtual void addEmpty(T* buffer){
        if (buffer!=nullptr)
            emptyBuffers->push(buffer);
    }

    virtual T* getEmpty(){
        T* buffer;
        emptyBuffers->pop(buffer);
        return buffer;
    }

    virtual void addFull(T* buffer){
        if (buffer!=nullptr)
            filledBuffers->push(buffer);
    }

    virtual T* getFull(){
        T* buffer;
        filledBuffers->pop(buffer);
        return buffer;
    }

    virtual int bufferLength(){
        return buffer_length;
    }

    protected:
        int buffer_count;
        int buffer_length;
        float amplifier;
        T clip;
        Queue<T*> *emptyBuffers;
        Queue<T*> *filledBuffers;

};

/**
 * @brief SoundBuffer where the data is standardized and amplified
 * 
 * @tparam T 
 */
template <class T> 
class SoundBuffer : public Buffers<T> {
  public:
    SoundBuffer(int buffer_count, int buffer_length, float amplifier, T clip) : Buffers<T>(buffer_count, buffer_length){
        this->amplifier = amplifier;
        this->clip = abs(clip);
    }

    T* getFull(){
        T* buffer = Buffers<T>::getFull();
        int len = Buffers<T>::bufferLength();
        // we make sure that the avarage is 0
        T avg_value = avg(buffer,len);
        // make sure that the avg is at 0 and amplify the values
        standardize(buffer,avg_value, len);
        return buffer;
    }

    T avg( T* buffer, int len){
        double total = 0;
        for (int j=0;j<len;j++){
            total += buffer[j];
        }
        return total / len;
    }

    void standardize(T* buffer,T avg_value , int len){
        for (int j=0;j<len;j++){
           T value =  (buffer[j] - avg_value) * amplifier;
           if (value>clip){
               value = clip;
           }
           if (value < -clip){
               value = -clip;
           }
            buffer[j] = value;
        }
    }

  protected:
    float amplifier;
    T clip;

};
