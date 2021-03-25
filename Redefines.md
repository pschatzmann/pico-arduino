# Defines

You can overwrite the default settings for the following defines with the help of cmake  e.g. you can define alternative
pins for Serial1 with:
```
set(ARDUINO_SKETCH_DEFINITIONS "-DSERIAL1_TX=12" "-DSERIAL1_RX=13")
```


## Pins
```
#define SS 17
```

### Serial
```
#define SERIAL1_TX 0
#define SERIAL1_RX 1
#define SERIAL2_TX 4
#define SERIAL2_RX 5
```

### I2C
```
#define I2C_SDA 12
#define I2C_SCL 13
#define I2C1_SDA 14
#define I2C1_SCL 15
```

### SPI 
```
#define SPI_RX 16
#define SPI_TX 19
#define SPI_CS 17
#define SPI_SCK 18
#define SPI1_RX 12
#define SPI1_TX 11
#define SPI1_CS 13
#define SPI1_SCK 10
```

## Other Topics

### Logging
```
#define PICO_LOG_LEVEL Error
```

### Servo
```
#define MIN_PULSE_WIDTH       544
#define MAX_PULSE_WIDTH      2400
#define DEFAULT_PULSE_WIDTH  1500
#define REFRESH_INTERVAL    20000
```

### PWM
```
#define PWM_MAX_NUMER 65535
#define PICO_ARDUINO_PWM_FREQUENCY 490
#define PWM_READ_REPEAT 10
```

### Arduino Main
Prevent the generation of main with loop and setup by defining
```
#define PICO_ARDUINO_NO_MAIN
```

