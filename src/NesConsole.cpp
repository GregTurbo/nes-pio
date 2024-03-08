#include "NesConsole.hpp"

#ifdef CFG_NES_CONFIG_FILE
#include CFG_NES_CONFIG_FILE
#else
#include "nes_config.h"
#endif

#ifdef CFG_NES_CONSOLE_COUNT

#define INVALID_INSTANCE 0xff

static NesConsole *_instances[CFG_NES_CONSOLE_COUNT] = {};

NesConsole::NesConsole(uint data_pin, uint clock_pin, uint latch_pin, PIO pio, int sm, int offset) {
    for (_instance = 0; _instance < CFG_NES_CONSOLE_COUNT; _instance++) {
        if (_instances[_instance] == nullptr) {
            _instances[_instance] = this;
            break;
        }
        _instance = INVALID_INSTANCE;
        return;
    }

    nes_device_port_init(&_port, data_pin, clock_pin, latch_pin, packet_size, pio, sm, offset);
    gpio_set_irq_enabled_with_callback(latch_pin, GPIO_IRQ_EDGE_RISE, true, &LatchIrqHandler);
}

NesConsole::~NesConsole() {
    nes_device_port_terminate(&_port);
    gpio_set_irq_enabled_with_callback(_port.latch_pin, 0, false, nullptr);
    if (_instance != INVALID_INSTANCE) {
        _instances[_instance] = nullptr;
    }
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

void NesConsole::LatchIrqHandler(uint gpio, uint32_t event_mask) {
    if (event_mask != GPIO_IRQ_EDGE_RISE) {
        return;
    }
    for (uint8_t i = 0; i < CFG_NES_CONSOLE_COUNT; i++) {
        NesConsole *console = _instances[i];
        if (console != nullptr && console->_port.latch_pin == gpio) {
            console->_report.reserved = 0xFF;
            nes_device_send_packet(&console->_port, console->_report.raw16);
            return;
        }
    }
}

#endif