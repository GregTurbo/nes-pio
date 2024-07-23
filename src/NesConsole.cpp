#include "NesConsole.hpp"

#ifdef CFG_NES_CONFIG_FILE
#include CFG_NES_CONFIG_FILE
#else
#include "nes_config.h"
#endif

#ifdef CFG_NES_CONSOLE_COUNT

#define INVALID_INSTANCE 0xff

static NesConsole *_instance = nullptr;
static uint8_t data = 0;

void __not_in_flash_func(LatchIrqHandler)() {
    gpio_put(data, 1);
    _instance->_report.reserved = 0xFF;
    nes_device_send_packet(&_instance->_port, _instance->_report.raw16);
    iobank0_hw->intr[data / 8] = 0xF << 4 * (data % 8);
}

NesConsole::NesConsole(uint data_pin, uint clock_pin, uint latch_pin, PIO pio, int sm, int offset) {
    if (_instance == nullptr) {
        _instance = this;
    }

    nes_device_port_init(&_port, data_pin, clock_pin, latch_pin, packet_size, pio, sm, offset);
    data = data_pin;
    gpio_set_irq_enabled(latch_pin, GPIO_IRQ_EDGE_RISE, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
    irq_add_shared_handler(IO_IRQ_BANK0, &PullHighPlease, 200);
}

NesConsole::~NesConsole() {
    nes_device_port_terminate(&_port);
    gpio_set_irq_enabled(latch_pin, GPIO_IRQ_EDGE_RISE, false);
    irq_set_enabled(IO_IRQ_BANK0, false);
    irq_remove_handler(IO_IRQ_BANK0, irq_get_exclusive_handler());
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