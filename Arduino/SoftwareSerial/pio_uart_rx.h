// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ----------- //
// pio_uart_rx //
// ----------- //

#define pio_uart_rx_wrap_target 0
#define pio_uart_rx_wrap 8

static const uint16_t pio_uart_rx_program_instructions[] = {
            //     .wrap_target
    0x2020, //  0: wait   0 pin, 0                   
    0xea27, //  1: set    x, 7                   [10]
    0x4001, //  2: in     pins, 1                    
    0x0642, //  3: jmp    x--, 2                 [6] 
    0x00c8, //  4: jmp    pin, 8                     
    0xc014, //  5: irq    nowait 4 rel               
    0x20a0, //  6: wait   1 pin, 0                   
    0x0000, //  7: jmp    0                          
    0x8020, //  8: push   block                      
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program pio_uart_rx_program = {
    .instructions = pio_uart_rx_program_instructions,
    .length = 9,
    .origin = -1,
};

static inline pio_sm_config pio_uart_rx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + pio_uart_rx_wrap_target, offset + pio_uart_rx_wrap);
    return c;
}
#endif

