#pragma once

#include <SdFat.h>
#include <algorithm>    // std::max
#include "pins_arduino.h"

/**
 * @brief A simple class to manage the WAV file names and assign new numbers
 */

#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)

class WAVFile {
    public:
        WAVFile(){
        }

        void begin() {
            if (!sd.begin(SS, SD_SCK_MHZ(1))){
                Serial.println("sd.begin() failed!");
            }
            fileCount = findMaxFileIndex();
        }

        // get a new non existing file name
        File32 &getNextFile() {
            file.close();
            sprintf(fileName, "/SN_%d.wav", ++fileCount);
            if (!file.open(fileName, O_RDWR | O_CREAT | O_TRUNC)){
                Serial.println("file.open() failed!");
            }
            return file;
        }

        const char* name() {
            return (const char*)fileName;
        }

    protected:
        SdFat32 sd;
        File32 file;
        int fileCount;
        char fileName[80];

        // we want to make sure that we generate new file names so we get the max assigned number
        int findMaxFileIndex() {
            int result = 0;
            File32 file;
            File32 dir;
            if (dir.open("/")){
                while (file.openNext(&dir, O_RDONLY)) {
                    const char * name = file.name();
                    Serial.println(name);
                    if (strncmp(name,"SN_" ,3)==0) {
                        int idx = atoi(name+3);
                        result = max(idx, result);
                    }
                    file.close();
                }
            }
            return result;
        }
};

