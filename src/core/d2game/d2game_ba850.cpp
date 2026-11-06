// 1.13c D2Game+0xBA850 quest unit spawn (retail asm: EBX and EDI live registers, stack w0 = key, w1, w2 = a record with the byte +0x11; ret 0xC):
// 0xEEF10(EAX = key on the first try, the previous result (0) on the next, ECX = &{ 10 dwords: +0 key, +4 EBX, +8 0, +0xC 0xF3, +0x10 1, +0x14 0, +0x18 w1,
// +0x1C EDI, +0x20 -1 / 5 / 10 (first, second, third try), +0x24 a 16-bit 0 }) is tried up to three times; the first non-zero unit gets its +0xC4 |= 0x3000000,
// the record's byte +0x11 = 1 and is returned; none: 0.
#include "d2game_kmem.h"

namespace d2g_ba850 {
struct Env {
    D2gMem m;
    uint32_t (*sEEF10)(uint32_t eax, uint32_t key, uint32_t ebx, uint32_t w1, uint32_t edi, uint32_t sel);   // the probe builds the 10-dword block
};
inline uint32_t run(const Env& e, uint32_t ebx, uint32_t edi, uint32_t key, uint32_t w1, uint32_t rec) {
    static const uint32_t sel[3] = { 0xFFFFFFFFu, 5, 0xA };
    uint32_t u = 0;
    for (int i = 0; i < 3; i++) {
        u = e.sEEF10(i == 0 ? key : 0, key, ebx, w1, edi, sel[i]);
        if (u) {
            e.m.w32(u + 0xC4, e.m.r32(u + 0xC4) | 0x3000000u);
            e.m.w8(rec + 0x11, 1);
            return u;
        }
    }
    return 0;
}
}
