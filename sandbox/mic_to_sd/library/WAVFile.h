#pragma once

#include <SdFat.h>
#include <algorithm>    // std::max


/**
 * @brief A simple class to manage the WAV file names and assign new numbers
 */

class WAVFile {
    public:
        WAVFile(){
            // open sd
            sd.begin();
            fileCount = findMaxFileIndex();
        }

        // get a new non existing file name
        SdFile getNextFile() {
            file.close();
            char fileName[40];
            sprintf(fileName, "SN_%d.wav", ++fileCount);
            file.open(fileName, O_RDWR | O_CREAT);
            return file;
        }

    protected:
        SdFat sd;
        SdFile file;
        int fileCount;

        // we want to make sure that we generate new file names so we get the max assigned number
        int findMaxFileIndex() {
            int result = 0;
            File file;
            File dir;
            if (dir.open("/")){
                while (file.openNext(&dir, O_RDONLY)) {
                    const char * name = file.name();
                    if (strncmp("","SN_" ,3)==0) {
                        int idx = atoi(name+3);
                        result = max(idx, result);
                    }
                    file.close();
                }
            }
            return result;
        }
};

