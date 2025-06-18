#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sstream>
#include <termios.h>
#include <thread>
#include <unistd.h>

#include "bus.h"
#include "cpu.h"

Bus system_6502;
std::map<uint16_t, std::string> asm_map;

std::string hex(uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
}

void clear_screen() {
    std::cout << "\033[2J\033[1;1H"; // ANSI escape code to clear screen
}

void display_ram(uint16_t address, int rows, int columns) {
    for (int row = 0; row < rows; row++) {
        std::cout << "$" << hex(address, 4) << ":";
        for (int col = 0; col < columns; col++) {
            std::cout << " " << hex(system_6502.read(address, true), 2);
            address += 1;
        }
        std::cout << "\n";
    }
}

void display_cpu() {
    std::cout << "STATUS: ";
    std::cout << ((system_6502.cpu.status & cpu6502::N) ? "N" : "n") << " ";
    std::cout << ((system_6502.cpu.status & cpu6502::V) ? "V" : "v") << " ";
    std::cout << ((system_6502.cpu.status & cpu6502::U) ? "U" : "u") << " ";
    std::cout << ((system_6502.cpu.status & cpu6502::B) ? "B" : "b") << " ";
    std::cout << ((system_6502.cpu.status & cpu6502::D) ? "D" : "d") << " ";
    std::cout << ((system_6502.cpu.status & cpu6502::I) ? "I" : "i") << " ";
    std::cout << ((system_6502.cpu.status & cpu6502::Z) ? "Z" : "z") << " ";
    std::cout << ((system_6502.cpu.status & cpu6502::C) ? "C" : "c") << "\n";

    std::cout << "PC: $" << hex(system_6502.cpu.pc, 4) << "\n";
    std::cout << "A:  $" << hex(system_6502.cpu.a, 2) << " [" << std::to_string(system_6502.cpu.a)
              << "]\n";
    std::cout << "X:  $" << hex(system_6502.cpu.x, 2) << " [" << std::to_string(system_6502.cpu.x)
              << "]\n";
    std::cout << "Y:  $" << hex(system_6502.cpu.y, 2) << " [" << std::to_string(system_6502.cpu.y)
              << "]\n";
    std::cout << "Stack P: $" << hex(system_6502.cpu.stkp, 4) << "\n";
}

void display_code(uint16_t pc, int lines) {
    auto it_a = asm_map.find(pc);
    if (it_a == asm_map.end())
        return;

    auto it = it_a;
    for (int i = 0; i < lines / 2; i++) {
        if (it == asm_map.begin())
            break;
        --it;
    }

    for (int i = 0; i < lines; i++) {
        std::string line = it->second;
        if (it->first == pc)
            std::cout << "> ";
        else
            std::cout << "  ";
        std::cout << line << "\n";
        if (++it == asm_map.end())
            break;
    }
}

void load_program() {
    std::stringstream ss;
    ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA "
          "8D 02 00 EA EA EA";
    uint16_t offset = 0x8000;
    while (!ss.eof()) {
        std::string b;
        ss >> b;
        system_6502.ram[offset++] = (uint8_t)std::stoul(b, nullptr, 16);
    }

    system_6502.ram[0xFFFC] = 0x00;
    system_6502.ram[0xFFFD] = 0x80;

    asm_map = system_6502.cpu.disassemble(0x0000, 0xFFFF);
    system_6502.cpu.reset();
}

char get_char_nonblocking() {
    struct termios oldt, newt;
    char ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // disable buffering
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return ch;
}

int main() {
    load_program();

    while (true) {
        clear_screen();

        std::cout << "RAM @ 0x0000:\n";
        display_ram(0x0000, 16, 16);

        std::cout << "\nRAM @ 0x8000:\n";
        display_ram(0x8000, 16, 16);

        std::cout << "\nCPU STATE:\n";
        display_cpu();

        std::cout << "\nCODE:\n";
        display_code(system_6502.cpu.pc, 10);

        std::cout << "\n[SPACE] Step  [R] Reset  [I] IRQ  [N] NMI  [Q] Quit\n";

        char key = 0;
        while ((key = get_char_nonblocking()) == -1)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (key == ' ') {
            do {
                system_6502.cpu.clock();
            } while (!system_6502.cpu.complete());
        } else if (key == 'r' || key == 'R') {
            system_6502.cpu.reset();
        } else if (key == 'i' || key == 'I') {
            system_6502.cpu.irq();
        } else if (key == 'n' || key == 'N') {
            system_6502.cpu.nmi();
        } else if (key == 'q' || key == 'Q') {
            break;
        }
    }

    return 0;
}
