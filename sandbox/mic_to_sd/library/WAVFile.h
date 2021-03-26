#pragma once

#include <SdFat.h>
#include <algorithm>    // std::max
#include "pins_arduino.h"

/**
 * @brief A simple class to manage the WAV file names and assign new numbers
 */

#define SPEED_MHZ 12
#define SD_CONFIG SdSpiConfig(SS, DEDICATED_SPI, SD_SCK_MHZ(SPEED_MHZ))

class WAVFile {
    public:
        WAVFile(){
        }

        void begin() {
            if (!sd.begin(SD_CONFIG)){
                Serial.println("sd.begin() failed!");
            }
            fileCount = findMaxFileIndex();
        }

        // get a new non existing file name
        File &getNextFile() {
            file.close();
            sprintf(fileName, "SN_%d.WAV", ++fileCount);
            if (!file.open(fileName, FILE_WRITE)) {
                Serial.println("file.open() failed!");
            }
            return file;
        }

        const char* name() {
            return (const char*)fileName;
        }

    protected:
        SdFat sd;
        File file;
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

