#pragma once

#include "PicoHardwareI2C.h"


namespace pico_arduino {

/**
 * @brief Provides the Wire object to support interfacing via I2C
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

inline PicoHardwareI2C Wire(i2c0, 160, I2C_SDA, I2C_SCL);  
inline PicoHardwareI2C Wire1(i2c1, 160, I2C1_SDA, I2C1_SCL);  

}