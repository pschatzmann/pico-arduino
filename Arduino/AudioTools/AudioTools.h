/**
 * @brief Some Generic Tools which might be useful: 
 * We provide 
 * - a Buffer and Double Buffer that can be used e.g. to manage PCM data
 * - a stream copy functionality
 * - an in memory Stream
 */

#pragma once
#include "Arduino.h"
#define DEFAULT_SAMLE_RATE 44100
#define DEFAULT_BUFFER_MAX_SIZE 512


/**
 * @brief Sound information which is available in the WAV header
 * 
 */
struct AudioInfo {
    int format;
    int sample_rate;
    int bits_per_sample;
    int channels;
    int byte_rate;
    int block_align;
    bool is_streamed;
    bool is_valid;
    uint32_t data_length;
    uint32_t file_size;
};

class AudioInfoSource {
    public:
         virtual AudioInfo &audioInfo() = 0;
};

class AudioInfoTarget {
    public:
         virtual void setAudioInfo(AudioInfoSource &ai) = 0;
};

class AudioOut : public Stream, public AudioInfoTarget {

};


/**
 * @brief Logger
 * 
 */
class AudioLogCls {
    public:
        void begin(Stream &stream){
            stream_ptr = &stream;
        }
        void info(const char* msg, const char *msg1=nullptr){
            if (stream_ptr!=nullptr){
                stream_ptr->print("Info: ");
                stream_ptr->print(msg);
                if (msg1!=nullptr)
                    stream_ptr->print(msg1);
                stream_ptr->println();
            }
        }
        void info(const char* msg, int code){
            if (stream_ptr!=nullptr){
                stream_ptr->print("Info: ");
                stream_ptr->print(msg);
                stream_ptr->print(code);
                stream_ptr->println();
            }
        }
        void error(const char* msg, const char* msg1=nullptr){
            if (stream_ptr!=nullptr){
                stream_ptr->print("Error: ");
                stream_ptr->print(msg);
                if (msg1!=nullptr)
                    stream_ptr->print(msg1);
                stream_ptr->println();
            }
        }
        void error(const char* msg, int code){
            if (stream_ptr!=nullptr){
                stream_ptr->print("Error: ");
                stream_ptr->print(msg);
                stream_ptr->print(code);
                stream_ptr->println();
            }
        }

    protected:
        Stream *stream_ptr;
};

extern AudioLogCls AudioLogger;

struct BufferResult {

}

/**
 * @brief a simple single buffer
 * 
 * @tparam T 
 */
template <class T>
class SingleBuffer {
  public:
    SingleBuffer(size_t size=DEFAULT_BUFFER_MAX_SIZE) {
        if (size>0){
            max_size = size;
            buffer = new T[size];
        }
    }


    ~SingleBuffer(){
        if (buffer!=nullptr)
            delete[] buffer;
    }

    // writes a single value to the buffer
    bool write(T value){
        bool result = false;

        // this should not really happen
        if (isFull()){
            AudioLogger.error("Buffer is full");
            return false;
        }

        // add entry to buffer
        buffer[buffer_pos]=value;
        if (buffer_pos<max_size) {
            buffer_pos++;
            result = true;
        } else {
            AudioLogger.error("Buffer Overflow");
        }

        // call registered handler if buffer is full: e.g to switch to an empty buffer
        if buffer_full_callback != nullptr && isFull()){
            buffer_full_callback(arg_ptr);
        }
        return result;
    }

    // provides a pointer to the data and the length in entries: this is efficient since we do not copy data
    size_t data(T* &resultBuffer, size_t requestedLen=DEFAULT_BUFFER_MAX_SIZE){
        size_t result_len = min(requestedLen, buffer_pos - (read_ptr - buffer));
        resultBuffer = read_ptr;
        read_ptr += result_len;
        return result_len;
    }


    // copies the data from the buffer into the result. The length is in number of entries. If there is no more data we return 0
     size_t read(T *result, size_t length=DEFAULT_BUFFER_MAX_SIZE) {
        size_t result_len = min(length, buffer_pos - (read_ptr - buffer));
        if (result_len>0){
            memcpy(result, read_ptr, result_len * sizeof(T));
            read_ptr += result_len;
        }
        return result_len;
    }

    // copies the data from the buffer into the result. The length is bytes
    virtual size_t readBytes(uint8_t *buffer, size_t length = DEFAULT_BUFFER_MAX_SIZE * sizeof(T)) {
        return read(buffer, length*sizeof(T)) * sizeof(T);
    }

    // number of entries
    size_t size() {
        return max_size;
    }

    // size in bytes
    size_t sizeBytes() {
        return max_size*sizeof(T);
    }

    // reset the buffer position
    void reset(){
        buffer_pos=0;
        read_ptr = buffer;
    }

    // available number of entries
    size_t available() {
        return buffer_pos;
    }

    // avalable bytes
    size_t availableBytes() {
        return buffer_pos*sizeof(T);
    }

    bool isFull() {
        return buffer_pos>=max_size;
    }

    // defines a handler which will be called when the buffer is full
    void setIsFullCallback(void (*buffer_full_callback)(void* ptr), void* argPtr = this){
        this->arg_ptr = argPtr;
        this->buffer_full_callback = buffer_full_callback;
    }


  protected:
    size_t max_size = 1024;
    size_t buffer_pos;
    T* buffer = nullptr;
    T* read_ptr = nullptr;
    void (*buffer_full_callback)(void* ptr);
    void* arg_ptr;
};



/**
 * @brief Helper class which Copies an input stream to a output stream
 * 
 */
class StreamCopy {
    public:
        StreamCopy(Stream &from, Stream &to, int buffer_size){
            this->from = &from;
            this->to = &to;
            this->buffer_size = buffer_size;
            buffer = new uint8_t[buffer_size];
        }

        ~StreamCopy(){
            delete[] buffer;
        }

        /// copies the available bytes from the input stream to the ouptut stream
        size_t copy() {
            size_t total_bytes = available();
            size_t result = total_bytes;
            while (total_bytes>0){
                size_t bytes_to_read = min(total_bytes,static_cast<size_t>(buffer_size) );
                from->readBytes(buffer, bytes_to_read);
                to->write(buffer, bytes_to_read);
                total_bytes -= bytes_to_read;
            }
            return result;
        }

        int available() {
            return from->available();
        }

    protected:
        Stream *from;
        Stream *to;
        uint8_t *buffer;
        int buffer_size;

};

/**
 * @brief A simple Stream implementation which is backed by allocated memory
 * 
 */
class MemoryStream : public Stream {
    public: 
        MemoryStream(int buffer_size = 512){
            this->buffer_size = buffer_size;
            this->buffer = new uint8_t[buffer_size];
            this->owns_buffer = true;
        }

        MemoryStream(const uint8_t *buffer, int buffer_size){
            this->buffer_size = buffer_size;
            this->write_pos = buffer_size;
            this->buffer = (uint8_t*)buffer;
            this->owns_buffer = false;
        }

        ~MemoryStream(){
            if (owns_buffer)
                delete[] buffer;
        }

        virtual size_t write(uint8_t byte) {
            int result = 0;
            if (write_pos<buffer_size){
                result = 1;
                buffer[write_pos] = byte;
                write_pos++;
            }
            return result;
        }

        virtual int available() {
            return write_pos - read_pos;
        }

        virtual int read() {
            int result = peek();
            if (result>0){
                read_pos++;
            }
            return result;
        }

        virtual int peek() {
            int result = -1;
            if (available()>0 && read_pos < write_pos){
                result = buffer[read_pos];
            }
            return result;
        }

        virtual void flush(){
        }

        void clear(bool reset=false){
            write_pos = 0;
            read_pos = 0;
            if (reset){
                // we clear the buffer data
                memset(buffer,0,buffer_size);
            }
        }


    protected:
        int write_pos;
        int read_pos;
        int buffer_size;
        uint8_t *buffer;
        bool owns_buffer;

};

/**
 * @brief Convert/Scale audio data 
 * 
 */
template <class InT, class OutT>
class Filter {
   public:
    Filter(float scale=1.0, float offset=0){
        this->scale = scale;
        this->offset = offset;
    }

    virtual OutT apply(InT value) {
        return value * scale + offset;
    };

   protected:
    float scale=1.0;
    float offset=0;
};

/**
 * @brief Output class which applys a filter before wriging the data to the final
 * stream
 * 
 * @tparam InT 
 * @tparam OutT 
 */

template <class InT, class OutT>
class FilteredStream {
    public:
        FilteredStream(Stream &out, Filter<InT,OutT> &filter){
            this->out = &out;
            this->filter = filter;
        }

        void write(void* data, int size){
            if (sizeof(InT)<=sizeof(OutT)){
                InT *in_data_ptr = data;
                OutT *out_data_ptr = data;
                int len = size / sizeof(InT);
                for (int j=0;j<len;j++){
                    out_data_ptr[j] = filter->apply(in_data_ptr[j]);
                }
                out->write((uint8_t*)data, size * sizeof(InT)/sizeof(OutT));
            } else {
                AudioLogger.error("Filtering not possible - The input size must be >= output size");
                out->write((uint8_t*)data, size);
            }
        }

    protected:
        Stream *out;
        Filter<InT,OutT> *filter;

};

/**
 * @brief Abstract class which defines the base class for audio producers
 * 
 */
class AudioSource {
    public:
        virtual size_t readBytes(uint8_t *buffer, size_t length);
};

/**
 * @brief Generates a Sound with the help of sin() function.
 * 
 */
template <class T>
class SineWaveGenerator  : public AudioInfoSource , public AudioSource {
    public:
        // the scale defines the max value which is generated
        SineWaveGenerator(double scale=1.0) {
            this->scale = scale;
        }

        void begin(uint16_t sample_rate=44100, uint16_t frequency=0){
            this->frequency = frequency;
            ai.sample_rate = sample_rate;
            ai.channels = 1;
            ai.bits_per_sample = sizeof(T)*8;
        }

        void setFrequency(uint16_t frequency) {
            this->frequency = frequency;
        }

        virtual size_t readBytes(uint8_t *buffer, size_t length){
            T *data = (T*) buffer;
            int size = length / sizeof(T);

            for (int j=0;j<size;j++){
                T value = sin(2.0 * PI * sample_step++ * frequency / ai.sample_rate) * scale;
                data[j] = value;
            }
            size_t result = size * sizeof(T);
//          AudioLogger.info("size: ",result);
          
            return result;
        }
        
        AudioInfo &audioInfo() {
            return ai;
        }

    protected:
        AudioInfo ai;
        uint32_t sample_step;
        uint16_t frequency;
        double scale;
};

/**
 * @brief Generates a Sound with the help of sin() function.
 * 
 */
template <class T>
class NoiseGenerator  :  public AudioSource {
    public:
        // the scale defines the max value which is generated
        NoiseGenerator(double scale=1.0) {
            this->scale = scale;
        }

        void begin(){
        }

        virtual size_t readBytes(uint8_t *buffer, size_t length){
            T *data = (T*) buffer;
            int size = length / sizeof(T);
            for (int j=0;j<size;j++){
                T value = (((rand() % 100) - 50) * 32767 / 50) * scale ; // generate number between  -32767 / 32767
                data[j] = value;
            }
            size_t result = size * sizeof(T);
            return result;
        }
        
    protected:
        double scale;
};

/**
 * @brief A simple Audiosource which reads the data from a Microphone which is connected to
 * an analog pin using the ADC
 * 
 */
class Microphone :  public AudioSource  {
    public:        
        Microphone(int numberOfBuffers=5, bufferSize=512){
            for (int j=0;j<numberOfBuffers;j++) {
                // allocate all buffers
                SingleBuffer new_buffer = new SingleBuffer<int16_t>(bufferSize);
                new_buffer.setIsFullCallback(callback_write_is_full, this);
                availableBuffers.push(new_buffer);
            }
            // current write buffer in use
            actualWrite = availableBuffers.pop();
        }

        ~Microphone(){
             end();
             while(availableBuffers.size()>0){
                 delete availableBuffers.pop();
             }
             if (actualWrite!=nullptr){
                 delete actualWrite;
             }
             if (actualRead!=nullptr){
                 delete actualRead;
             }
         }

        // starts the processing
        void begin(uint16_t sample_rate=44100, int pinNumber=GP26, int core=1, int adc = 0){
            this->sample_rate = sample_rate;
            adc_init();
            // Select ADC input 0 (GPIO26)
            adc_gpio_init(pinNumber);
            adc_select_input(adc);

            // start caputre with timer on core 1
            timeInUs = 1000000000 / sample_rate;
            if (core==1){
                //multicore_launch_core1(callback_timer_start);
                thread.start(callback_timer_start, this);
            } else {
                // start timer on core 0
                callback_timer_start(this);
            }
        }

        // stops the processing
        void end(){
            // stop timer
            timer.end();
            // clear existing data    
            Queue<SingleBuffer<int16_t>*> tmp;
            for (int j=0;j<filledBuffers.size();j++){
                tmp = filledBuffers.pop();
                tmp.reset();
                availableBuffers.push(tmp);
            }

            // reset actual write buffer
            if (actualWrite!=nullptr)
                actualWrite.reset();

            // reset actual read buffer
            if (actualRead!=nullptr)
                actualRead.reset();

        }   

        // copy the available data into the buffer
        virtual size_t readBytes(uint8_t *buffer, size_t length){
            int resultLen = 0;
            if (actualRead==nullptr){
                actualRead = filledBuffers.pop();
            }
            // get next buffer
            if (actualRead.available()<=0){
                // make the buffer available again
                actualRead.reset();
                availableBuffers.push(actualRead);
                // get data from next fulled buffer
                actualRead = filledBuffers.pop();
            }
            resultLen = actualRead.readBytes(buffer, length);
        }

        // writes a single sample
        void writeSample(int16_t value){
            actualWrite->write(value);
        }

        // provides the sample rate
        uint16_t sampleRate(){
            return sample_rate;
        }

        // callback method which samples an individual value
        static bool callback_capture(repeating_timer_t *rt) {
            Microphone *ptr = (Microphone*) rt->user_data;
            uint16_t value = adc_read();
            ptr->writeSample(value - 0xFFFF / 2 );
            return true;
        }

        // callback method which starts the timer
        static void callback_timer_start(void *ptr) {
            Microphone *self = (Microphone *) ptr;
            timer.start(callback_capture, self->timeInUs, US, this);
        }

        // callback meethod which is called when the actual write buffer is full
        static void callback_write_is_full(void *ptr) {
            Microphone *self = (Microphone *) ptr;
            self->onWriteIsFull();
        }

        // logic to handle logic when the actual write buffer is full
        void onWriteIsFull(){
            filledBuffers.push(actualWrite);
            if (availableBuffers.size()==0){
                AudioLogger.error("Buffer overflow - ignoring oldest buffer values");
                actualWrite = filledBuffers.pop();
                actualWrite.reset();
            } else {
                // switch to next empty buffer
                actualWrite = availableBuffers.pop();
            }
        }


    protected:
        TimerAlarmRepeating timer;
        Thread thread; // start writer on processor 1
        Queue<SingleBuffer<int16_t>*> availableBuffers(100, true, true);
        Queue<SingleBuffer<int16_t>*> filledBuffers(100, true, true);
        SingleBuffer<int16_t>* actualWrite; // only accessed by writer processor
        SingleBuffer<int16_t>* actualRead; // only accessed by reader processor
        uint16_t sample_rate;
        long timeInUs;

};