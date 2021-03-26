#include "Arduino.h"
#include <SdFat.h>
#include "library/WavEncoder.h"
#include "library/WavFile.h"

const int sampling_rate = 20; //20000;
const int num_channels = 1;
const int bits_per_sample = sizeof(int16_t) * 8;
const int buffer_count = 10;
const int buffer_length = 1024;
const int save_every_n_min = 1;
const pin_size_t mic_pin = 27;

Queue<int16_t*> emptyBuffers(buffer_count, true, true);
Queue<int16_t*> filledBuffers(buffer_count, true, true);
Thread thread;
TimerAlarmRepeating timer;
WAVFile wavFile;
WAVEncoder wavEncoder(num_channels, sampling_rate, bits_per_sample);

// measure effective sampling rate
uint64_t start_time;
uint64_t sample_count;



// read a sample from the microphone into the buffer
bool microphoneRead(repeating_timer_t *rt) {
    // we keep the following data between the calls
    static int pos = buffer_length;
    static int16_t *buffer;

    // when the buffer is full get new buffer
    if (pos==buffer_length){
        //Serial.println("using next buffer");
        // make data available to write to SD card
        if (buffer!=nullptr){
            filledBuffers.push(buffer);
        }
        // get new empty buffer
        emptyBuffers.pop(buffer);
        pos = 0;
    }

    // add a sample to the buffer
    buffer[pos++] = analogRead(mic_pin);

    // set up data to measure effective sampling rate
    sample_count++;
    if (start_time==0){
        start_time = millis();
    }
    return true;
}

// calcualte the sampling rate = samples / time
uint64_t measuredSamplingRate() {
   return (millis() == start_time) ? 0 : sample_count * 1000 / (millis() - start_time);
}

// callback for logic which is processed on processor 1
void writeDataThread(void*) {
    Serial.println("writeDataThread started");
    int32_t timeout = 0;
    File file;
    while(true) {
        // create a new file every 10 minutes
        if (millis()>timeout){
            Serial.println();
            Serial.println("Determining next file...");
            file = wavFile.getNextFile();
            Serial.print("Processing ");
            Serial.println(wavFile.name());
            wavEncoder.setSampleRate(measuredSamplingRate());
            wavEncoder.begin(file);
            timeout = millis() + (1000 * 60 * save_every_n_min);
        }

        // get next data to write
        int16_t *data;
        if (filledBuffers.pop(data)){
            Serial.print(".");
            // write the data to the sd drive
            wavEncoder.write(data, buffer_length* sizeof(int16_t));   
            // make the buffer available again
            emptyBuffers.push(data);
        }
    }
}


// Setup Sound processing
void setup(){
    Serial.begin(115200);
    while(!Serial){
        delay(50);
    }
    Serial.println("setup...");
    Logger.begin(Serial,PicoLogger::Error);

    // allocate buffers
    for (int j=0;j<buffer_count;j++) {
        int16_t* new_buffer = new int16_t[buffer_length];
        emptyBuffers.push(new_buffer);
    }

    //wavFile.begin();

    // start the saving to the SD drive on processor 1
    thread.start(writeDataThread);

    // start sampling of sound from microphone with 44100 Hz
    uint64_t correction = 16;
    uint64_t time = (1000000 / sampling_rate) - correction;
    timer.start(microphoneRead, time, US);
}



// Just print out the calculated total sampling rate every 10 seconds
void loop() {
    //Serial.print("Sampling rate: ");
    //Serial.println(measuredSamplingRate());
    //delay(10000);
}