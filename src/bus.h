#pragma once
#include <cstdint>
#include <array>

#include "cpu.h"

class Bus {
public:
    // Constructor and Deconstructor
    Bus();
    ~Bus();

    // Devices
    cpu6502 cpu;
    std::array<uint8_t, 64 * 1024> ram;

    // Read And Write
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr, bool bReadOnly = false);
};
