#ifndef _NES_NES_DEFINITIONS_H
#define _NES_NES_DEFINITIONS_H

#include <pico/stdlib.h>

typedef union {
    uint16_t raw16 = 0;

    struct __attribute__((packed)) {
        bool a : 1;
        bool b : 1;
        bool select : 1;
        bool start : 1;
        bool dpad_up : 1;
        bool dpad_down : 1;
        bool dpad_left : 1;
        bool dpad_right : 1;
        uint8_t reserved : 8;
    };
} nes_report_t;

#endif