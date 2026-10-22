// src/core/storm/storm_batch_b56a.cpp -- Storm.dll CRT helpers from the cached decompiles.
// Skipped: 0x34C40 (Win32 time calls), 0x21B30/0x21BC0 (callee 0x21780 unmapped), 0x225B0 (SEH),
// 0x22AB0 (register args), 0x3A870/0x2D020/0x2BBB0 (implicit EAX/thiscall register state), 0x3870 (SEH).
#include <initializer_list>
#include <cstdint>
#include <cstdarg>
#include <cstdio>

// 1.13c Storm+0x0553B
extern "C" int storm__vsprintf(char* dest, const char* fmt, va_list ap) {
    if (!dest) return vsnprintf(nullptr, 0, fmt, ap);
    return vsnprintf(dest, 0x7fffffff, fmt, ap);
}

// 1.13c Storm+0x05801
extern "C" int storm___vsnprintf(char* dest, size_t count, const char* fmt, va_list ap) {
    if (!dest) return vsnprintf(nullptr, 0, fmt, ap);
    int n = vsnprintf(dest, count, fmt, ap);
    return (n < 0 || (size_t)n >= count) ? -1 : n;
}

// 1.13c Storm+0x0E930
extern "C" uint64_t storm___allmul(uint64_t a, uint64_t b) { return a * b; }
