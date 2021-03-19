
/**
 * @brief Provide compatibility with avr/pgmspace.h
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#include "deprecated-avr-comp/avr/pgmspace.h"

// add some missing functions which are not provided by the Arduino framework
#define vsnprintf_P vsnprintf 