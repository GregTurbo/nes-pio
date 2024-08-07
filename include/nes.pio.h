// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --------- //
// nes_write //
// --------- //

#define nes_write_wrap_target 1
#define nes_write_wrap 3

#define nes_write_T1 1

#define nes_write_offset_write 0u

static const uint16_t nes_write_program_instructions[] = {
    0xe081, //  0: set    pindirs, 1                 
            //     .wrap_target
    0x6001, //  1: out    pins, 1                    
    0x2020, //  2: wait   0 pin, 0                   
    0x21a0, //  3: wait   1 pin, 0               [1] 
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program nes_write_program = {
    .instructions = nes_write_program_instructions,
    .length = 4,
    .origin = -1,
};

static inline pio_sm_config nes_write_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + nes_write_wrap_target, offset + nes_write_wrap);
    return c;
}

#include <hardware/clocks.h>
static inline pio_sm_config nes_write_program_get_config(PIO pio, uint sm, uint offset, uint data_pin, uint clock_pin, uint packet_size) {
    pio_gpio_init(pio, data_pin);
    pio_sm_config c = nes_write_program_get_default_config(offset);
    sm_config_set_out_pins(&c, data_pin, 1);
    sm_config_set_set_pins(&c, data_pin, 1);
    sm_config_set_in_pins(&c, clock_pin);
    // Shift to right, autopull based on packet_size
    sm_config_set_out_shift(&c, true, true, packet_size);
    int delay_cycles = nes_write_T1;
    int delay_nanos = 100;
    int target_clock = delay_cycles / (delay_nanos * 1E-9);
    float div = clock_get_hz(clk_sys) / target_clock;
    sm_config_set_clkdiv(&c, div);
    return c;
}
static inline void nes_write_program_init(
    PIO pio,
    uint sm,
    uint offset,
    uint data_pin,
    uint clock_pin,
    pio_sm_config *c
) {
    pio_sm_init(pio, sm, offset, c);
    pio_sm_set_enabled(pio, sm, true);
}

#endif
