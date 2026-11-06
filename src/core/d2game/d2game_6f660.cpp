// 1.13c D2Game+0x6F660, injectable callees. EAX = unit (type 0 or 1), stack word 0 = missile (type 3) (ret 4); EBX is not read.
// 0x1250 (EAX = missile class id) returns a record whose byte +4 is tested against gdwBitMasks[2] (byte at bm+8). #10910 (esi,0x9c,0) and
// #10973 (esi,0xa6,0) give two chances; their sum is the roll threshold. #10587 (esi,0x148,0) is the stat fed to #10614 (ECX = &seed, EDX = stat),
// which fills the 64-bit seed local. Up to four LCG draws (x*0x6AC690C5 + hi) % 100 < sum count; the count goes to #10887 (missile,0x148,n,0).
#include <cstdint>
#include <cstring>
struct G6f660Env {
    uint32_t (*rec1250)(uint32_t eaxClassId);
    uint32_t (*a802)(uint32_t unit, uint32_t id, uint32_t z);
    uint32_t (*a42a)(uint32_t unit, uint32_t id, uint32_t z);
    uint32_t (*a3e2)(uint32_t unit, uint32_t id, uint32_t z);
    void (*a628)(uint32_t* seed, uint32_t edxStat);
    void (*a472)(uint32_t unit, uint32_t id, uint32_t v, uint32_t z);
};
static uint32_t rd32(uint32_t a, uint32_t off) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + off), 4); return v; }
// 1.13c D2Game+0x6F660
void g_6f660(uint32_t eax, uint32_t missile, uint32_t bm, const G6f660Env& env)
{
    if (eax == 0 || missile == 0) return;
    if (rd32(missile, 0) != 3) return;
    const uint32_t t = rd32(eax, 0);
    if (t != 0 && t != 1) return;
    const uint32_t rec = env.rec1250(rd32(missile, 4));
    if (((rd32(bm, 8) & 0xFF) & (rd32(rec, 4) & 0xFF)) == 0) return;
    const uint32_t a = env.a802(eax, 0x9c, 0);
    const uint32_t sum = a + env.a42a(eax, 0xa6, 0);
    if (sum == 0) return;
    uint32_t seed[2];
    env.a628(seed, env.a3e2(eax, 0x148, 0));
    uint32_t lo = seed[0], hi = seed[1];
    uint32_t n = 0;
    while ((int32_t)n < 4) {
        const uint64_t x = (uint64_t)lo * 0x6AC690C5u + hi;
        lo = (uint32_t)x; hi = (uint32_t)(x >> 32);
        if ((int32_t)(lo % 100u) >= (int32_t)sum) break;
        ++n;
    }
    env.a472(missile, 0x148, n, 0);
}
