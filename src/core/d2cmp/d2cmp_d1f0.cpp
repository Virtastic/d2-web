// src/core/d2cmp/d2cmp_d1f0.cpp -- 1.13c D2CMP sub_D1F0 (ordinal 10037), transcribed from the disassembly.
#include <cstdint>

struct D2CmpD1F0Env {
    void (*vsprintf_)(char* buf, const char* fmt, void* args);   // 0x2A4B
    const char* strA;     // PTR_DAT_6fe32ccc
    const char* strB;     // PTR_DAT_6fe32cd0
    uint32_t flagC8;      // DAT_6fe32cc8
};

// cdecl(buf, appendA, fmt, va_list): formats into buf, then appends strA when appendA != 0 and flagC8 != 0, else strB.
// 1.13c D2CMP+0x0D1F0
void d2cmp_d1f0(char* buf, int32_t appendA, const char* fmt, void* args, const D2CmpD1F0Env* e) {
    e->vsprintf_(buf, fmt, args);
    const char* s = (appendA != 0 && e->flagC8 != 0) ? e->strA : e->strB;
    char* d = buf;
    while (*d) d++;
    do { *d++ = *s; } while (*s++);
}
