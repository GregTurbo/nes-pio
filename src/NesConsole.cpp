#include "NesConsole.hpp"
#include "Arduino.h"

#ifdef CFG_NES_CONFIG_FILE
#include CFG_NES_CONFIG_FILE
#else
#include "nes_config.h"
#endif

#ifdef CFG_NES_CONSOLE_COUNT

#define INVALID_INSTANCE 0xff

static NesConsole *_instance = nullptr;
static uint8_t data = 0;
static uint8_t latch = 0;

bool LED_OK = false;
bool SCREEN_OK = false;

uint64_t counter = 0;

//really not a fan but this hack works
void __not_in_flash_func(LatchIrqHandler)() {
    gpio_put(8, 1);
    _instance->_report.reserved = 0xFF;
    nes_device_send_packet(&_instance->_port, _instance->_report.raw16);
    gpio_acknowledge_irq(10, GPIO_IRQ_EDGE_RISE);
    if(counter % 2 == 0) {
        LED_OK = true;
    } else {
        SCREEN_OK = true;
    }
    counter += 1;
}

NesConsole::NesConsole(uint data_pin, uint clock_pin, uint latch_pin, PIO pio, int sm, int offset) {
    if (_instance == nullptr) {
        _instance = this;
    }

    nes_device_port_init(&_port, data_pin, clock_pin, latch_pin, packet_size, pio, sm, offset);
    data = data_pin;
    latch = latch_pin;
    gpio_add_raw_irq_handler_with_order_priority(10, &LatchIrqHandler, PICO_HIGHEST_IRQ_PRIORITY);
    gpio_set_irq_enabled(latch_pin, GPIO_IRQ_EDGE_RISE, true);
    irq_set_priority(IO_IRQ_BANK0, PICO_HIGHEST_IRQ_PRIORITY);
}

NesConsole::~NesConsole() {
    nes_device_port_terminate(&_port);
    gpio_set_irq_enabled(_port.latch_pin, GPIO_IRQ_EDGE_RISE, false);
    gpio_remove_raw_irq_handler(_port.data_pin, &LatchIrqHandler);
    //irq_set_enabled(IO_IRQ_BANK0, false);
    _instance = nullptr;
}

bool NesConsole::Detect() {
    return _latch_triggered;
}

void NesConsole::SendReport(nes_report_t &report) {
    _report = report;
}

int NesConsole::GetOffset() {
    return _port.offset;
}

#endif

