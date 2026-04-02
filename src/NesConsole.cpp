#include "NesConsole.hpp"

#ifdef CFG_NES_CONFIG_FILE
#include CFG_NES_CONFIG_FILE
#else
#include "nes_config.h"
#endif

#ifdef CFG_NES_CONSOLE_COUNT

#define INVALID_INSTANCE 0xff

static NesConsole *_instance = nullptr;

//not good, fix later
void __not_in_flash_func(LatchIrqHandler)() {
    gpio_put(8, 1);
    _instance->_report.reserved = 0xFF;
    nes_device_send_packet(&_instance->_port, _instance->_report.raw16);
    gpio_acknowledge_irq(10, GPIO_IRQ_EDGE_RISE);
}

NesConsole::NesConsole(uint data_pin, uint clock_pin, uint latch_pin, PIO pio, int sm, int offset) {
    if (_instance == nullptr) {
        _instance = this;
    }

    nes_device_port_init(&_port, data_pin, clock_pin, latch_pin, packet_size, pio, sm, offset);
    gpio_add_raw_irq_handler_with_order_priority(10, &LatchIrqHandler, 255);
    gpio_set_irq_enabled(latch_pin, GPIO_IRQ_EDGE_RISE, true);
}

NesConsole::~NesConsole() {
    nes_device_port_terminate(&_port);
    gpio_set_irq_enabled(_port.latch_pin, GPIO_IRQ_EDGE_RISE, false);
    gpio_remove_raw_irq_handler(_port.data_pin, &LatchIrqHandler);
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
