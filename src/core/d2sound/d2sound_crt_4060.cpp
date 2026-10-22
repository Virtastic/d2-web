// src/core/d2sound/d2sound_crt_4060.cpp -- D2Sound.dll CRT helpers (_memset, __callnewh, _unlock(4) wrapper).
#include <cstdint>
#include <cstddef>

void d2s_unlock(int);

// 1.13c D2sound+0x1D0B  _unlock(4)
void d2s_unlock4() { d2s_unlock(4); }

// 1.13c D2sound+0x403F  __callnewh(size): runs the installed new-handler; 1 if it asked for a retry
int (*d2s_pnh)(size_t) = nullptr;
int d2s_callnewh(size_t size) {
    if (d2s_pnh != nullptr && d2s_pnh(size) != 0) return 1;
    return 0;
}

// 1.13c D2sound+0x4060  _memset(dst, val, n): byte fill, returns dst
void* d2s_memset(void* dst, int val, size_t n) {
    uint8_t* p = (uint8_t*)dst;
    while (n--) *p++ = (uint8_t)val;
    return dst;
}
