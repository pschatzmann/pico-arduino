#pragma once
#include "Arduino.h"

#define WAV_FORMAT_PCM 0x0001

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

/**
 * @brief A simple WAV file encoder. 
 * 
 */
class WAVEncoder {
    public: 

        WAVEncoder(int input_channels=2, int input_sample_rate=44100, int input_bits_per_sample=16, int maxSamples=0) {
            audioInfo.channels = input_channels;
            audioInfo.sample_rate = input_sample_rate;
            audioInfo.bits_per_sample = input_bits_per_sample;
            setFileSize();
            if (max_samples>0){
                audioInfo.file_size = max_samples * audioInfo.bits_per_sample / 8 * audioInfo.channels;
                streaming = false;
            } else {
                audioInfo.file_size = 0xffffffff;
                streaming = true;
            }
        }

        void begin(Stream &out){
            stream_ptr = &out;
            writeRiffHeader();
            writeFMT();
            writeDataHeader();
            stream_ptr->flush();
        }

        void setSampleRate(int rate){
            audioInfo.sample_rate = rate;
        }

        void setDataFormat(uint16_t format = WAV_FORMAT_PCM) {
            audioInfo.format = format;
        }

        void setFileSize(uint32_t length=0xffffffff) {
            audioInfo.file_size = length;
            this->max_samples = 0;
        }

        void setMaxSamples(uint32_t samples) {
            this->max_samples = samples;
        }

        virtual size_t write(void *in_ptr, size_t in_size){
            int32_t result = 0;
            if (streaming){
                result = stream_ptr->write((uint8_t*)in_ptr, in_size);
            }  else if (audioInfo.file_size>0){
                size_t write_size = min((size_t)in_size,(size_t)audioInfo.file_size);
                result = stream_ptr->write((uint8_t*)in_ptr, write_size);
                audioInfo.file_size -= write_size;
            }
            return result;
        }

        void end(){
            stream_ptr->flush();
        }

    protected:
        Stream* stream_ptr;
        AudioInfo audioInfo;
        uint32_t max_samples;
        bool streaming;

        void writeRiffHeader(){
            stream_ptr->write("RIFF",4);
            stream_ptr->write(audioInfo.file_size-8);
            stream_ptr->write("WAVE",4);
        }

        void writeFMT(){
            uint16_t fmt_len = 16;
            uint32_t byteRate = audioInfo.sample_rate * audioInfo.bits_per_sample * audioInfo.channels / 8;
            uint32_t frame_size = audioInfo.channels * audioInfo.bits_per_sample / 8;
            stream_ptr->write("FMT ",4);
            stream_ptr->write(fmt_len);
            stream_ptr->write(audioInfo.format); //PCM
            stream_ptr->write(audioInfo.channels); 
            stream_ptr->write(audioInfo.sample_rate); 
            stream_ptr->write(byteRate); 
            stream_ptr->write(frame_size);  //frame size
            stream_ptr->write(audioInfo.bits_per_sample);             
        }

        void writeDataHeader() {
            stream_ptr->write("DATA",4);
            audioInfo.file_size -=44;
            stream_ptr->write(audioInfo.file_size);
        }

};
