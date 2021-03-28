#pragma once
#include "PicoQueue.h"

namespace pico_arduino {

/**
 * @brief We are managing a 2 collections of memory arrays: One for the available buffers which can be requested
 * to record data and the buffers of filled data which need to be processed (e.g. stored to a SD drive) 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

template <class T> 
class Buffers {
  public:
    /**
     * @brief Construct a new Buffers object
     * 
     * @param buffer_count 
     * @param buffer_length 
     * @param blocking 
     * @param synchronized 
     */
    Buffers(int buffer_count, int buffer_length, bool blocking=true, bool synchronized = true){
        this->buffer_count = buffer_count;
        this->buffer_length = buffer_length;

        emptyBuffers = new Queue<T*>(buffer_count, blocking, synchronized);
        filledBuffers = new Queue<T*>(buffer_count, blocking, synchronized);

        // allocate buffers
        for (int j=0;j<buffer_count;j++) {
            T* new_buffer = new T[buffer_length];
            emptyBuffers->push(new_buffer);
        }
    }

    /// Destructor
     ~Buffers(){
         if (emptyBuffers!=nullptr) delete emptyBuffers;
         if (filledBuffers!=nullptr) delete filledBuffers;
     }

    /// Makes the buffer available as empty
    virtual void addEmpty(T* buffer){
        if (buffer!=nullptr)
            emptyBuffers->push(buffer);
    }

    /// Retrieves an empty buffer
    virtual T* getEmpty(){
        T* buffer;
        emptyBuffers->pop(buffer);
        return buffer;
    }

    /// Adds the buffer to the list of available data
    virtual void addFull(T* buffer){
        if (buffer!=nullptr)
            filledBuffers->push(buffer);
    }

    /// Retrieves the next available data
    virtual T* getFull(){
        T* buffer;
        filledBuffers->pop(buffer);
        return buffer;
    }

    /// Provides the length of each buffer
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
 * @brief SoundBuffer where the data is standardized, amplified and clipped
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 * @tparam T 
 */
template <class T> 
class SoundBuffer : public Buffers<T> {
  public:
    /**
     * @brief Construct a new Sound Buffer object
     * 
     * @param buffer_count number of buffers
     * @param buffer_length number of entries in one buffer
     * @param amplifier factor by which each sample is multiplied
     * @param clip amplified values above this value are clipped (clipping is deactive if value is <=0)
     */
    SoundBuffer(int buffer_count, int buffer_length, double amplifier = 1.0, T clip=0) : Buffers<T>(buffer_count, buffer_length, true, true){
        this->amplifier = amplifier;
        this->clip = abs(clip);
    }

    /// provides the buffer which contains the sound samples - the recorded values are standardized
    T* getFull(){
        T* buffer = Buffers<T>::getFull();
        int len = Buffers<T>::bufferLength();
        // we make sure that the avarage is 0
        T avg_value = avg(buffer,len);
        // make sure that the avg is at 0 and amplify the values
        standardize(buffer,avg_value, len);
        return buffer;
    }

  protected:
    double amplifier;
    T clip;

    /// calculate the average over all samples
    double avg( T* buffer, int len){
        double total = 0;
        for (int j=0;j<len;j++){
            total += buffer[j];
        }
        return total / len;
    }

    /// make sure that the center is at 0 and that the peaks are clipped
    void standardize(T* buffer,double avg_value , int len){
        if (clip>=0){
            for (int j=0;j<len;j++){
                T value =  (static_cast<double>(buffer[j]) - avg_value) * amplifier;
                if (value>clip){
                    value = clip;
                }
                if (value < -clip){
                    value = -clip;
                }
                buffer[j] = value;
            }
        } else {
            for (int j=0;j<len;j++){
                buffer[j] =  (static_cast<double>(buffer[j]) - avg_value) * amplifier;
            }
        }
    }

};

} // namespace
