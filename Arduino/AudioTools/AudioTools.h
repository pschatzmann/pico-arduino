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
         virtual AudioInfo &audioInfo() {
             return ai;
         }

    protected:
        AudioInfo ai;
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
class AudioLogger {
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

        static AudioLogger &instance(){
            static AudioLogger* inst;
            if (inst==nullptr){
                inst = new AudioLogger();
            }
            return *inst;
        }

    protected:
        Stream *stream_ptr;
};


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

        // determine max value (for potential scaling)
        if (value>max_value){
            max_value = value;
        }

        // this should not really happen
        if (isFull()){
            AudioLogger::instance().error("Buffer is full");
            return false;
        }

        // add entry to buffer
        buffer[buffer_write_pos]=value;
        if (buffer_write_pos<max_size) {
            buffer_write_pos++;
            result = true;
        } else {
            AudioLogger::instance().error("Buffer Overflow");
        }

        // call registered handler if buffer is full: e.g to switch to an empty buffer
        if (buffer_full_callback != nullptr && isFull()){
            buffer_full_callback(arg_ptr);
        }
        return result;
    }

    // provides a pointer to the data and the length in entries: this is efficient since we do not copy data
    size_t data(T* &resultBuffer, size_t requestedLen=DEFAULT_BUFFER_MAX_SIZE){
        size_t result_len = min(requestedLen, buffer_write_pos - (read_ptr - buffer));
        resultBuffer = read_ptr;
        read_ptr += result_len;
        return result_len;
    }

    // provides a pointer to the data and the length in entries: this is efficient since we do not copy data
    size_t dataBytes(uint8_t* &resultBuffer, size_t requestedLen=DEFAULT_BUFFER_MAX_SIZE * sizeof(T)){
        size_t result_len = min(requestedLen /sizeof(T) , buffer_write_pos - (read_ptr - buffer));
        resultBuffer = (uint8_t*) read_ptr;
        read_ptr += result_len;
        return result_len * sizeof(T);
    }

    // copies the data from the buffer into the result. The length is in number of entries. If there is no more data we return 0
     size_t read(T *result, size_t length=DEFAULT_BUFFER_MAX_SIZE) {
        size_t result_len = min(length, buffer_write_pos - (read_ptr - buffer));
        if (result_len>0){
            memcpy(result, read_ptr, result_len * sizeof(T));
            read_ptr += result_len;
        }
        return result_len;
    }

    // copies the data from the buffer into the result. The length is bytes
    virtual size_t readBytes(uint8_t *buffer, size_t length = DEFAULT_BUFFER_MAX_SIZE * sizeof(T)) {
        return read((T*)buffer, length*sizeof(T)) * sizeof(T);
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
        buffer_write_pos = 0;
        read_ptr = buffer;
        max_value = 0;
    }

    // available number of entries
    size_t available() {
        return buffer_write_pos - (read_ptr-buffer);
    }

    // avalable bytes
    size_t availableBytes() {
        return available()*sizeof(T);
    }

    bool isFull() {
        return buffer_write_pos>=max_size;
    }

    T max(){
        return max_value;
    }

    // defines a handler which will be called when the buffer is full
    void setIsFullCallback(void (*buffer_full_callback)(void* ptr), void* argPtr = nullptr){
        this->arg_ptr = argPtr == nullptr ? this :  argPtr;
        this->buffer_full_callback = buffer_full_callback;
    }


  protected:
    size_t max_size = 1024;
    size_t buffer_write_pos = 0;
    T* buffer = nullptr;
    T* read_ptr = nullptr;
    T max_value=0;
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
                AudioLogger::instance().error("Filtering not possible - The input size must be >= output size");
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
                double value = sin(2.0 * PI * sample_step++ * frequency / ai.sample_rate) * scale;
                data[j] = value;
            }
            size_t result = size * sizeof(T);          
            return result;
        }

        virtual size_t data(T* &data, size_t len=512){
            static const int MAX = 512;
            static T buffer[MAX];
            size_t result_size = max(len, MAX);
            readBytes((uint8_t *)buffer, result_size * sizeof(T));
            data = buffer;
            return result_size;            
        }

        
    protected:
        uint64_t sample_step;
        uint16_t frequency;
        double scale;
};

/**
 * @brief Generates a Sound with the help of sin() function.
 * 
 */
template <class T>
class NoiseGenerator  :  public AudioSource, public AudioInfoSource {
    public:
        // the scale defines the max value which is generated
        NoiseGenerator(double scale=1.0) {
            this->scale = scale;
        }

        void begin(){
            ai.channels = 1;
            ai.bits_per_sample = sizeof(T)*8;
        }

        virtual size_t readBytes(uint8_t *buffer, size_t length){
            T *data = (T*) buffer;
            int size = length / sizeof(T);
            for (int j=0;j<size;j++){
                T value = ((rand() % (static_cast<T>(2 * scale)) - scale)); // generate number between  -scale / scale
                data[j] = value;
            }
            size_t result = size * sizeof(T);
            return result;
        }

        virtual size_t data(T* &data, size_t len=512){
            static const int MAX = 512;
            static T buffer[MAX];
            size_t result_size = max(len, MAX);
            readBytes((uint8_t *)buffer, result_size * sizeof(T));
            data = buffer;
            return result_size;            
        }

    protected:
        double scale;

};

/**
 * @brief A simple Audiosource which reads the data from a Microphone which is connected to
 * an analog pin using the ADC
 * 
 */
class Microphone :  public AudioSource, public AudioInfoSource  {
    public:        
        Microphone(uint16_t bufferSize=512, uint16_t numberOfBuffers=5){
            this->buffer_size = bufferSize;
            for (int j=0;j<numberOfBuffers;j++) {
                // allocate all buffers
                auto new_buffer = new SingleBuffer<int16_t>(bufferSize);
                if (new_buffer!=nullptr){
                    new_buffer->setIsFullCallback(callback_write_is_full, this);
                    availableBuffers.push(new_buffer);
                } else {
                    AudioLogger::instance().error("Could not allocate all buffers");
                }
            }
            // current write buffer in use
            availableBuffers.pop(actualWrite);
        }

        ~Microphone(){
             end();
             while(availableBuffers.size()>0){
                SingleBuffer<int16_t>* tmp;
                availableBuffers.pop(tmp);
                delete tmp;
             }
             if (actualWrite!=nullptr){
                 delete actualWrite;
             }
             if (actualRead!=nullptr){
                 delete actualRead;
             }
         }

        // starts the processing
        void begin(uint16_t sample_rate=44100, int pinNumber=GP26, int core=1 ){
            this->sample_rate = sample_rate;
            ai.sample_rate = sample_rate;
            ai.channels = 1;
            ai.bits_per_sample = sizeof(int16_t)*8;

            // we determine the ADC
            int adc = pinToADC(pinNumber);
            if (adc>=0) {
                adc_init();
                // Select ADC input 0 (GPIO26)
                adc_gpio_init(pinNumber);
                adc_select_input(adc);
                //free-running sampling mode.
                adc_run(true);

                if (core==1){
                    //multicore_launch_core1(callback_timer_start);
                    thread.start(callback_timer_start, this);
                } else {
                    // start timer on core 0
                    callback_timer_start(this);
                }
                startMs = millis();
                active = true;
            }
        }

        // stops the processing
        void end(){
            // stop timer
            timer.stop();
            // clear existing data    
            SingleBuffer<int16_t> *tmp;
            for (int j=0;j<filledBuffers.size();j++){
                filledBuffers.pop(tmp);
                tmp->reset();
                availableBuffers.push(tmp);
            }

            // reset actual write buffer
            if (actualWrite!=nullptr)
                actualWrite->reset();

            // reset actual read buffer
            if (actualRead!=nullptr)
                actualRead->reset();

            active = false;
        }   

        // copy the available data into the buffer
        virtual size_t readBytes(uint8_t *buffer, size_t length){
            int resultLen = 0;
            if (active){
                if (getNextReadBuffer()) {
                    resultLen = actualRead->readBytes(buffer, length);
                }
            }
            return resultLen;
        }

        // returns the data und the size in int16 entries
        size_t data(int16_t* &result){
            int resultLen = 0;
            if (active){
                if (getNextReadBuffer()) {
                    resultLen = actualRead->data(result);
                }
            }
            return resultLen;
        }

        // we provide direct access to the read buffer 
        SingleBuffer<int16_t> *buffer() {
            return actualRead;
        }

        // writes a single sample
        void writeSample(int16_t value){
            if (active){
                actualWrite->write(value);
                sampleCount++;
            }
        }

        // provides the sample rate
        uint16_t sampleRate(){
            return sample_rate;
        }

        // determines the effective sample rate
        double sampleRateEffective(){
            unsigned long timeMs = millis() - startMs;;
            return sampleCount * 1000.0 / timeMs;
            startMs = millis();
            sampleCount = 0;

        }

        // provide the max size of an individual buffer
        uint16_t bufferSize() {
            return buffer_size;
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
            long timeInUs = 1000000.0 / (self->sample_rate + 3000);
            // start capture with timer on core 1 in microsecondes
            self->timer.start(callback_capture, timeInUs, US, self);
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
                AudioLogger::instance().error("Buffer overflow - ignoring oldest buffer values");
                filledBuffers.pop(actualWrite);
                actualWrite->reset();
            } else {
                // switch to next empty buffer
                availableBuffers.pop(actualWrite);
            }
        }

        TimerAlarmRepeating timer; // timer which triggers samples

    protected:
        Thread thread; // start writer on processor 1
        Queue<SingleBuffer<int16_t>*> availableBuffers = Queue<SingleBuffer<int16_t>*>(100, true, true);
        Queue<SingleBuffer<int16_t>*> filledBuffers = Queue<SingleBuffer<int16_t>*>(100, true, true);
        SingleBuffer<int16_t>* actualWrite; // only accessed by writer processor
        SingleBuffer<int16_t>* actualRead; // only accessed by reader processor
        uint16_t sample_rate;
        unsigned long startMs;
        unsigned long sampleCount;
        int buffer_size;
        bool active = false;

        // determine the ADC
        int pinToADC(int pin){
            switch(pin){
                case 26:
                    return 0;
                case 27:
                    return 1;
                case 28:
                    return 2;
                default:
                    AudioLogger::instance().error("Only GPIO26, GPIO27 and GPIO28 are supported");
                    return -1;
            }
            return -1;
        }

        // make sure that we have data available - returns true if there is data available
        bool getNextReadBuffer(){
            bool result = false;
            if (actualRead==nullptr){
                filledBuffers.pop(actualRead);
            }
            // if everthing has been consumed
            if (actualRead->available()<=0){
                // make the buffer available again
                actualRead->reset();
                if (availableBuffers.size()>0){
                    availableBuffers.push(actualRead);
                    // get data from next fulled buffer
                    filledBuffers.pop(actualRead);
                    result = true;
                }
            } else {
                // there is still some data left
                result = true;
            }
            return result;
        }

};