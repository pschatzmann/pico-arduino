#pragma once
#include "PicoHardwareSPI.h"

namespace pico_arduino {

/**
 * @brief Provides the SPI and SPI1 object to support interfacing via SPI
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

inline PicoHardwareSPI SPI(spi0);
inline PicoHardwareSPI SPI1(spi1);

}