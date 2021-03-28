#include "Arduino.h"
#include <SdFat.h>
#include "Buffers.h"

const pin_size_t mic_pin = 27;
const int sd_speed_mhz = 12;
const int sampling_rate = -44100;
const int buffer_count = 10;
const int buffer_length = 1024;
const int save_every_n_min = 1;
const int amplify = 50;
const int clip = 10000;

SdFat sd;
Thread thread;
TimerAlarmRepeating timer;
SoundBuffer<int16_t> soundBuffer(buffer_count, buffer_length, amplify, clip);

// to measure the effective sampling rate
uint64_t start_time;
uint64_t sample_count;


// read a sample from the microphone into the buffer
bool microphoneRead(repeating_timer_t *rt) {
    // we keep the following data between the calls
    static int pos = buffer_length;
    static int16_t *buffer;

    // when the buffer is full get new buffer
    if (pos==buffer_length){
        // make bull buffer available to save
        soundBuffer.addFull(buffer);
        // get new empty buffer
        buffer = soundBuffer.getEmpty();
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
    char fileName[50];
    uint32_t fileCount = 0;
    long timeout = 0;

    Serial.println("Writing thread started on processor 1");

    File file;
    while(true) {
        // create a new file every 10 minutes
        if (millis()>timeout){
            Serial.println();
            if (file) file.close();
            sprintf(fileName, "SN_%d.WAV", ++fileCount);
            Serial.printf("Processing next file: %s\n", fileName);
            if (!file.open(fileName, O_WRONLY | O_CREAT | O_TRUNC)) {
                Serial.println("ERROR: Could not open the file!");
            }
            timeout = millis() + (1000 * 60 * save_every_n_min);
        }

        // get next data to write
        int16_t *data = soundBuffer.getFull();
        // write the data to the sd drive
        file.write(data, buffer_length* sizeof(int16_t));   
        // make the buffer available again
        soundBuffer.addEmpty(data);
        // we print a . for every saved buffer
        Serial.print("+");
    }
}


// Setup Sound processing
void setup(){
    Serial.begin(115200);
    Logger.begin(Serial,PicoLogger::Error);
    //while(!Serial);

    Serial.println("setup...");

    // setup CD
    if (!sd.begin(SdSpiConfig(SS, DEDICATED_SPI, SD_SCK_MHZ(sd_speed_mhz)))){
        Serial.println("sd.begin() failed!");
    }

    // start the saving to the SD drive on processor 1
    thread.start(writeDataThread);

    // start sampling of sound from microphone 
    uint64_t time = (1000000 / sampling_rate) ;
    timer.start(microphoneRead, time, US);
}


// Just print out the calculated total sampling rate every 10 seconds
void loop() {
    delay(10000);
    Serial.print("Sampling rate: ");
    Serial.print(measuredSamplingRate());
}