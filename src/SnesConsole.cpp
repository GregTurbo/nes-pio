#include "SnesConsole.hpp"
#include "nes.h"

#ifdef CFG_NES_CONFIG_FILE
#include CFG_NES_CONFIG_FILE
#else
#include "nes_config.h"
#endif

#ifdef CFG_SNES_CONSOLE_COUNT

#define INVALID_INSTANCE 0xff

static SnesConsole *_instance = nullptr;
static uint8_t data = 0;

void __not_in_flash_func(SnesLatchIrqHandler)() {
    gpio_put(data, 1);
    if(gpio_get_irq_event_mask(data) & GPIO_IRQ_EDGE_RISE) {
        gpio_acknowledge_irq(data, GPIO_IRQ_EDGE_RISE);
    }
    _instance->_report.reserved = 0;
    _instance->_report.end = 0xFFFF;
    snes_device_send_packet(&_instance->_port, _instance->_report.raw32);
    iobank0_hw->intr[data / 8] = 0xF << 4 * (data % 8);
}

SnesConsole::SnesConsole(
    uint data_pin,
    uint clock_pin,
    uint latch_pin,
    PIO pio,
    int sm,
    int offset
) {
    if (_instance == nullptr) {
        _instance = this;
    }

    nes_device_port_init(&_port, data_pin, clock_pin, latch_pin, packet_size, pio, sm, offset);
    data = data_pin;
    gpio_set_irq_enabled(latch_pin, GPIO_IRQ_EDGE_RISE, true);
    gpio_add_raw_irq_handler(data_pin, &SnesLatchIrqHandler);
    irq_set_enabled(IO_IRQ_BANK0, true);
    //gpio_set_irq_enabled_with_callback(latch_pin, GPIO_IRQ_EDGE_RISE, true, &LatchIrqHandler);
}

SnesConsole::~SnesConsole() {
    nes_device_port_terminate(&_port);
    gpio_set_irq_enabled(_port.latch_pin, GPIO_IRQ_EDGE_RISE, false);
    irq_set_enabled(IO_IRQ_BANK0, false);
    _instance = nullptr;
}

bool SnesConsole::Detect() {
    return _latch_triggered;
}

void SnesConsole::SendReport(snes_report_t &report) {
    _report = report;
}

int SnesConsole::GetOffset() {
    return _port.offset;
}

#endif
