#pragma once

#define BUILTIN_LED  PICO_DEFAULT_LED_PIN // backward compatibility
#define LED_BUILTIN PICO_DEFAULT_LED_PIN

#define GP0 0
#define GP1 1
#define GP2 2
#define GP3 3
#define GP4 4
#define GP5 5
#define GP6 6
#define GP7 7
#define GP8 8
#define GP9 9
#define GP10 10
#define GP11 11
#define GP12 12
#define GP13 13
#define GP14 14
#define GP15 15
#define GP16 16
#define GP17 17
#define GP18 18
#define GP19 19
#define GP20 20
#define GP21 21
#define GP22 22
#define GP23 23
#define GP24 23
#define GP25 24
#define GP26 26
#define GP27 27
#define GP28 28
#define GP29 29

// Some applications expect on the Arduino SPI SS
#ifndef SS
#define SS 17
#endif


// The following defines are use by the arduino-pico framwork and can be redefined if necessary

// ----
// Serial1: UART0 tx/rx = gp0/gp1; 

#ifndef SERIAL1_TX
#define SERIAL1_TX 0
#endif

#ifndef SERIAL1_RX
#define SERIAL1_RX 1
#endif

// Serial2: UART1 tx/rx = gp4/gp5; 

#ifndef SERIAL2_TX
#define SERIAL2_TX 4
#endif

#ifndef SERIAL2_RX
#define SERIAL2_RX 5
#endif

// ----
// PicoHardwareI2C Wire(i2c0, 160, GP12, GP13);  

#ifndef I2C_SDA
#define I2C_SDA 12
#endif

#ifndef I2C_SCL
#define I2C_SCL 13
#endif

// PicoHardwareI2C Wire1(i2c1, 160, GP14, GP15);  
#ifndef I2C1_SDA
#define I2C1_SDA 14
#endif

#ifndef I2C1_SCL
#define I2C1_SCL 15
#endif

// ----
//  spi0:  pinRx = 16; pinTx = 19; pinCS = 17; pinSCK = 18;

#ifndef SPI_RX
#define SPI_RX 16
#endif

#ifndef SPI_TX
#define SPI_TX 19
#endif

#ifndef SPI_CS
#define SPI_CS 17
#endif

#ifndef SPI_SCK
#define SPI_SCK 18
#endif

//  spi1:  pinRx = 12; pinTx = 11; pinCS = 13; pinSCK = 10;
#ifndef SPI1_RX
#define SPI1_RX 12
#endif

#ifndef SPI1_TX
#define SPI1_TX 11
#endif

#ifndef SPI1_CS
#define SPI1_CS 13
#endif

#ifndef SPI1_SCK
#define SPI1_SCK 10
#endif



