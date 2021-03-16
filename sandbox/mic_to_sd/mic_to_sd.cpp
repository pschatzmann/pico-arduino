#include "Arduino.h"
#include <SdFat.h>
#include "library/WavEncoder.h"
#include "library/WavFile.h"

const int sampling_rate = 44100;
const int num_channels = 1;
const int bits_per_sample = sizeof(int16_t) * 8;
const int buffer_count = 3;
const int buffer_lengh = 512;
const pin_size_t mic_pin = 10;

Queue<int16_t*> emptyFifo(buffer_count, true, true);
Queue<int16_t*> filledFifo(buffer_count, true, true);
Thread thread;
TimerAlarmRepeating timer;

// measure effective sampling rate
uint64_t start_time;
uint64_t sample_count;


// read a sample from the microphone into the buffer
bool microphoneRead(repeating_timer_t *rt) {
    // we keep the following data between the calls
    static int pos=buffer_lengh;
    static int16_t *buffer;

    // when the buffer is full get new buffer
    if (pos==buffer_lengh){
        // make data available to write to SD card
        if (buffer!=nullptr){
            filledFifo.push(buffer);
        }
        // get new empty buffer
        emptyFifo.pop(buffer);
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

// callback for logic which is processed on processor 1
void writeDataThread(void*) {
    int32_t timeout = 0;
    WAVFile wavFile;
    WAVEncoder wavEncoder(num_channels, sampling_rate, bits_per_sample);

    while(true) {
        // create a new file every 10 minutes
        if (millis()>timeout){
            SdFile file = wavFile.getNextFile();
            Serial.print("Processing ");
            Serial.println(file.name());
            wavEncoder.begin(file);
            timeout = millis() + 1000 * 60 * 10;
        }

        // get next data to write
        int16_t *data;
        if (filledFifo.pop(data)){
            // write the data to the sd drive
            wavEncoder.write(data, buffer_lengh * sizeof(int16_t));   
            // make the buffer available again
            emptyFifo.push(data);
        }

    }
}

// Setup Sound processing
void setup(){
    Serial.begin(115200);

    // allocate buffers
    for (int j=0;j<buffer_count;j++) {
        int16_t* new_buffer = new int16_t[buffer_lengh];
        emptyFifo.push(new_buffer);
    }

    // start the saving to the SD drive on processor 1
    thread.start(writeDataThread);

    // start sampling of sound from microphone with 44100 Hz
    uint64_t time = 1000000 / sampling_rate;
    timer.start(microphoneRead, time, US);
}


// calcualte the sampling rate = samples / time
uint64_t measuredSamplingRate() {
   return sample_count * 1000 / (millis() - start_time);
}

// Just print out the calculated total sampling rate every 10 seconds
void loop() {
    Serial.print("Sampling rate: ");
    Serial.println(measuredSamplingRate());
    delay(10000);
}