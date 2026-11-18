// src/core/storm/storm_batch_b56a.cpp -- Storm.dll CRT helpers from the cached decompiles.
// Skipped: 0x34C40 (Win32 time calls), 0x21B30/0x21BC0 (callee 0x21780 unmapped), 0x225B0 (SEH),
// 0x22AB0 (register args), 0x3A870/0x2D020/0x2BBB0 (implicit EAX/thiscall register state), 0x3870 (SEH).
#include <initializer_list>
#include <cstdint>
#include <cstdarg>
#include <cstdio>

// Storm+0x0553B _vsprintf and Storm+0x05801 __vsnprintf: src/core/crt/storm_vprintf.cpp.

// 1.13c Storm+0x0E930
extern "C" uint64_t storm___allmul(uint64_t a, uint64_t b) { return a * b; }
