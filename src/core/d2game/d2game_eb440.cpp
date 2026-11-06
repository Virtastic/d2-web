// 1.13c D2Game+0xEB440, injectable callee. Registers: EAX = end of the buffer, EBX = pointer to the read cursor; stack word 0 = unit
// (handed to the D2Common thunk 0xA4C0, 1 word); ret 4. The record 0xA4C0 returns holds three double-indirect tables at +0x60, +0x64
// and +0x68; each table is two pointers, and each pointer's first word points at a pair of dwords that receives two packet words.
#include <cstdint>
struct GEB440Env {
    uint32_t (*a4c0)(uint32_t unit);
};
static inline uint32_t* eb440w(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static void eb440put(uint32_t rec, uint32_t off, const uint32_t* s, int a, int b, int c, int d)
{
    const uint32_t t = *eb440w(rec + off);
    uint32_t* d0 = eb440w(*eb440w(*eb440w(t)));
    d0[0] = s[a]; d0[1] = s[b];
    uint32_t* d1 = eb440w(*eb440w(*eb440w(t + 4)));
    d1[0] = s[c]; d1[1] = s[d];
}
// 1.13c D2Game+0xEB440
uint32_t g_eb440(uint32_t eax, uint32_t ebx, uint32_t unit, const GEB440Env& env)
{
    const uint32_t cur = *eb440w(ebx);
    if (cur + 0x34 > eax || *(uint16_t*)(uintptr_t)cur != 0x7701) return 0x11;
    const uint32_t rec = env.a4c0(unit);
    const uint32_t* s = eb440w(cur);
    eb440put(rec, 0x60, s, 1, 2, 7, 8);
    eb440put(rec, 0x64, s, 3, 4, 9, 10);
    eb440put(rec, 0x68, s, 5, 6, 11, 12);
    *eb440w(ebx) = cur + 0x34;
    return 0;
}
