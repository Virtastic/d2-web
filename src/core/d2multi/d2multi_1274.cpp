// 1.13c D2Multi+0x1274 strtok: delimiter bitmap; the continuation pointer is the calling thread's ptd+0x18 (_token),
// reached through retail's __getptd (0x2E01). The slot holds a 32-bit address in the capture's address space.
#include <cstdint>

uint8_t* d2multi_strtok_1274(uint8_t* str, const uint8_t* delim, uint32_t* tokenSlot) {
    uint8_t map[32] = {};
    uint8_t b;
    do { b = *delim++; map[b >> 3] |= (uint8_t)(1 << (b & 7)); } while (b);
    if (!str) str = (uint8_t*)(uintptr_t)*tokenSlot;
    while ((map[*str >> 3] & (1 << (*str & 7))) && *str) ++str;
    uint8_t* p = str;
    for (;; ++p) {
        if (!*p) break;
        if (map[*p >> 3] & (1 << (*p & 7))) { *p++ = 0; break; }
    }
    *tokenSlot = (uint32_t)(uintptr_t)p;
    return str != p ? str : nullptr;
}
