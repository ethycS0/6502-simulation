#include <chrono>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <termios.h>
#include <thread>
#include <unistd.h>

#include "bus.h"
#include "cpu.h"

Bus nes;
std::map<uint16_t, std::string> mapAsm;

std::string hex(uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
}

void ClearScreen() {
    std::cout << "\033[2J\033[1;1H"; // ANSI escape code to clear screen
}

void DrawRam(uint16_t nAddr, int nRows, int nColumns) {
    for (int row = 0; row < nRows; row++) {
        std::cout << "$" << hex(nAddr, 4) << ":";
        for (int col = 0; col < nColumns; col++) {
            std::cout << " " << hex(nes.read(nAddr, true), 2);
            nAddr += 1;
        }
        std::cout << "\n";
    }
}

void DrawCpu() {
    using namespace cpu6502;

    std::cout << "STATUS: ";
    std::cout << ((nes.cpu.status & N) ? "N" : "n") << " ";
    std::cout << ((nes.cpu.status & V) ? "V" : "v") << " ";
    std::cout << ((nes.cpu.status & U) ? "U" : "u") << " ";
    std::cout << ((nes.cpu.status & B) ? "B" : "b") << " ";
    std::cout << ((nes.cpu.status & D) ? "D" : "d") << " ";
    std::cout << ((nes.cpu.status & I) ? "I" : "i") << " ";
    std::cout << ((nes.cpu.status & Z) ? "Z" : "z") << " ";
    std::cout << ((nes.cpu.status & C) ? "C" : "c") << "\n";

    std::cout << "PC: $" << hex(nes.cpu.pc, 4) << "\n";
    std::cout << "A:  $" << hex(nes.cpu.a, 2) << " [" << std::to_string(nes.cpu.a) << "]\n";
    std::cout << "X:  $" << hex(nes.cpu.x, 2) << " [" << std::to_string(nes.cpu.x) << "]\n";
    std::cout << "Y:  $" << hex(nes.cpu.y, 2) << " [" << std::to_string(nes.cpu.y) << "]\n";
    std::cout << "Stack P: $" << hex(nes.cpu.stkp, 4) << "\n";
}

void DrawCode(uint16_t pc, int nLines) {
    auto it_a = mapAsm.find(pc);
    if (it_a == mapAsm.end())
        return;

    auto it = it_a;
    for (int i = 0; i < nLines / 2; i++) {
        if (it == mapAsm.begin())
            break;
        --it;
    }

    for (int i = 0; i < nLines; i++) {
        std::string line = it->second;
        if (it->first == pc)
            std::cout << "> ";
        else
            std::cout << "  ";
        std::cout << line << "\n";
        if (++it == mapAsm.end())
            break;
    }
}

void LoadProgram() {
    std::stringstream ss;
    ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA "
          "8D 02 00 EA EA EA";
    uint16_t offset = 0x8000;
    while (!ss.eof()) {
        std::string b;
        ss >> b;
        nes.ram[offset++] = (uint8_t)std::stoul(b, nullptr, 16);
    }

    nes.ram[0xFFFC] = 0x00;
    nes.ram[0xFFFD] = 0x80;

    mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);
    nes.cpu.reset();
}

char GetCharNonBlocking() {
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
    LoadProgram();

    while (true) {
        ClearScreen();

        std::cout << "RAM @ 0x0000:\n";
        DrawRam(0x0000, 16, 16);

        std::cout << "\nRAM @ 0x8000:\n";
        DrawRam(0x8000, 16, 16);

        std::cout << "\nCPU STATE:\n";
        DrawCpu();

        std::cout << "\nCODE:\n";
        DrawCode(nes.cpu.pc, 10);

        std::cout << "\n[SPACE] Step  [R] Reset  [I] IRQ  [N] NMI  [Q] Quit\n";

        char key = 0;
        while ((key = GetCharNonBlocking()) == -1)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (key == ' ') {
            do {
                nes.cpu.clock();
            } while (!nes.cpu.complete());
        } else if (key == 'r' || key == 'R') {
            nes.cpu.reset();
        } else if (key == 'i' || key == 'I') {
            nes.cpu.irq();
        } else if (key == 'n' || key == 'N') {
            nes.cpu.nmi();
        } else if (key == 'q' || key == 'Q') {
            break;
        }
    }

    return 0;
}
