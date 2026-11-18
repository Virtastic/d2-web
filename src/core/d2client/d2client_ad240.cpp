// 1.13c D2Client+0xAD240: no arguments. i = low byte of 0xC1EA(0xC1DE(0xC1F0(local player at D2Client+0x11BBFC))) (each stdcall 1); wsprintfA(buf,
// "%s\%s", "DATA\GLOBAL", entry) with entry = the string pointer at D2Client+0xF9EB4[i]; then 0x69240(D2Client+0x1144C8, buf) (ret 8). EAX is that result.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ad240(const D2cSeam* s, uint32_t local, const uint32_t* names /* 256 pointers at D2Client+0xF9EB4 */) {
    char buf[0x104];
    uint64_t r = (*s)(0xc1f0, 0, 0, 0, 0, 0, local);
    r = (*s)(0xc1de, 0, 0, 0, 0, 0, r);
    r = (*s)(0xc1ea, 0, 0, 0, 0, 0, r);
    const char* a = "DATA\\GLOBAL"; const char* b = (const char*)(uintptr_t)names[(uint8_t)r];
    char* o = buf; while ((*o = *a++)) o++;
    *o++ = '\\'; while ((*o++ = *b++)) {}
    return (uint32_t)(*s)(0x69240, 0, 0, 0, 0, 0, s->mod(0x1144c8), (uint64_t)(uintptr_t)buf);
}
