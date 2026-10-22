// src/core/d2multi/d2multi_batch_b57a.cpp -- D2Multi.dll CRT leaves (batch b57-mul-a).
// Not ported: __onexit/_atexit/_free/sub_3AAC (CRT lock + heap seams), __SEH_prolog/epilog (register ABI),
// sub_35FE (__crtLCMapStringA: Win32 + SEH + alloca), __dosmaperr (retail errtable not recoverable here).
#include <initializer_list>
#include <cstdint>
#include <cstring>

// Single-threaded: _getptd() collapses to one static block (_terrno +0x08, _tdoserrno +0x0C).
static uint8_t g_b57a_ptd[0x8C];

// 1.13c D2Multi+0x35B0  ___ascii_stricmp: ASCII-fold compare, -1/0/1
int d2multi_b57a_ascii_stricmp(const char* a, const char* b) {
    for (;;) {
        uint8_t x = (uint8_t)*a++, y = (uint8_t)*b++;
        if (x != y) {
            uint8_t fx = (uint8_t)(x - 'A') < 26 ? (uint8_t)(x + 0x20) : x;
            uint8_t fy = (uint8_t)(y - 'A') < 26 ? (uint8_t)(y + 0x20) : y;
            if (fx != fy) return fx < fy ? -1 : 1;
        }
        if (y == 0) return 0;
    }
}

// 1.13c D2Multi+0x33C9  __RTC_Initialize: the init-table range is empty (start == end), so no calls
void d2multi_b57a_rtc_initialize(void) {}

// 1.13c D2Multi+0x39D4  &__getptd()->_terrno
int* d2multi_b57a_errno_ptr(void) { return (int*)(g_b57a_ptd + 0x08); }

// 1.13c D2Multi+0x39DD  &__getptd()->_tdoserrno
unsigned long* d2multi_b57a_doserrno_ptr(void) { return (unsigned long*)(g_b57a_ptd + 0x0C); }
