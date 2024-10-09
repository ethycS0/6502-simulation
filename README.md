# NES Emulator

## Introduction
A few months ago, the Elden Ring DLC dropped, and as a huge fan of FromSoftware games, I was all over it. I’ve thoroughly enjoyed Elden Ring and Sekiro, but there’s always been one game on my list: Bloodborne. Unfortunately, since I don’t own a PlayStation, I haven’t been able to play it. That’s what initially got me looking into emulators. I was amazed by the impressive development progress on the ShadPS4 emulator, and it sparked my interest in learning how to build emulators myself.

This NES emulator is my first project to dip my toes into emulator development. It’s both a personal project and a way to showcase my skills as a low-level developer during my undergraduate journey.

## Project Overview
This project aims to emulate the NES (Nintendo Entertainment System) in C++. While I’m following various resources to learn and implement this emulator, I am focusing on gaining a deep understanding of system architecture, CPU design (6502), and memory management. I’m not developing my own engine for display at the moment; instead, I am using the **olcPixelGameEngine** by OneLoneCoder for visualization purposes. In the future, I plan on transitioning to **SDL** for better control and flexibility.

### Features
- **CPU Emulation**: Based on the 6502 processor, implementing opcodes and addressing modes as they appear in the NES architecture.
- **Memory Management**: Correct handling of memory mapping, including the PPU, APU, and cartridge data.
- **Basic Display**: Using olcPixelGameEngine for rendering.
- **Planned Features**: Audio processing, input handling, and transition to SDL for more advanced rendering capabilities.

### Current Progress
The project is still in its early stages, with primary focus on the CPU implementation and ensuring correct instruction handling. Progress is ongoing as I continue to work on refining the 6502 CPU, memory mapping, and eventually, the display.
