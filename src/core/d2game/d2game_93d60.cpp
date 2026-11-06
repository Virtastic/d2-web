// 1.13c D2Game+0x93D60 and +0x94370 -- the two ring-spawn loops, rebuilt from the retail disassembly.
// 93D60: `edx` = low seed (SEED_InitLowSeed #10614 with ECX = &seed on the stack: lo = edx, hi = 0x29A), eight stack words
// (game, unit, p3, p4, p5, p6, p7, step; ret 0x20). A 23-dword request is zeroed, then {[0] = 3, [1] = unit, [4] = p3, [5] = p4, [6] = p5, [0xB] = p6,
// [0xC] = p7, [0x15] = callback (0x91040 here, 0x90EA0 in 94370), [7]/[8]/[0x16] per step}. Each step advances the seed
// (lo*0x6AC690C5 + hi, 64-bit) and calls createMissile 0x6F930 (ECX = game, EDX = request). 93D60 then, when it returned a unit and
// p3 == 0x238, calls #10637 (unit, baseLevel + 1); 94370 calls #10637 unconditionally. baseLevel is 0 when sgptDataTables+0xBA0 <= 0x118
// (signed) or sgptDataTables+0xB98 + 0x271A0 == 0, else the dword at (that sum + 0x14C). Pointers are 32-bit words read by byte offset.
#include <cstdint>
#include <cstring>

struct D2G93D60Env {
    uint32_t gTables;   // module global at D2Game+0xF829C: the address of the word holding the data-table block pointer
    uint32_t (*createMissile)(uint32_t game, const uint32_t* req);   // 0x6F930: ECX game, EDX request (23 dwords)
    void (*setLevel)(uint32_t unit, uint32_t level);                 // #10637 (stdcall, 2 words)
    uint32_t (*fnAddr)(uint32_t rva);                                // the address of the module routine at this D2Game RVA (the missile callbacks)
};
namespace {
const int32_t kTab[128] = {0,2,5,8,11,14,16,19,21,23,24,26,27,28,29,29,30,29,29,28,27,26,24,23,21,19,16,14,11,8,5,2,0,-2,-5,-8,-11,-14,-16,-19,-21,-23,-24,-26,-27,-28,-29,-29,-30,-29,-29,-28,-27,-26,-24,-23,-21,-19,-16,-14,-11,-8,-5,-2,30,29,29,28,27,26,24,23,21,19,16,14,11,8,5,2,0,-2,-5,-8,-11,-14,-16,-19,-21,-23,-24,-26,-27,-28,-29,-29,-30,-29,-29,-28,-27,-26,-24,-23,-21,-19,-16,-14,-11,-8,-5,-2,0,2,5,8,11,14,16,19,21,23,24,26,27,28,29,29};
inline uint32_t rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline uint32_t baseLevel(const D2G93D60Env& e) {
    const uint32_t dt = rd(e.gTables);
    if ((int32_t)rd(dt + 0xBA0) <= 0x118) return 0;
    const uint32_t b = rd(dt + 0xB98) + 0x271A0u;
    return b ? rd(b + 0x14C) : 0;
}
void ring(const D2G93D60Env& e, uint32_t seed, uint32_t game, uint32_t unit, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t step, uint32_t cb, bool gate) {
    uint32_t req[0x17] = {0};
    req[0] = 3; req[1] = unit; req[4] = p3; req[5] = p4; req[6] = p5; req[0xB] = p6; req[0xC] = p7; req[0x15] = cb;
    uint32_t lo = seed, hi = 0x29A;
    const uint32_t stride = step << 2;
    for (uint32_t o = 0;;) {
        req[7] = (uint32_t)kTab[0x40 + (o >> 2)];
        req[8] = (uint32_t)kTab[o >> 2];
        const uint64_t m = (uint64_t)lo * 0x6AC690C5u + hi;
        lo = (uint32_t)m; hi = (uint32_t)(m >> 32);
        req[0x16] = lo;
        const uint32_t u = e.createMissile(game, req);
        if (!gate || (u && p3 == 0x238)) e.setLevel(u, baseLevel(e) + 1);
        o += stride;
        if ((int32_t)o >= 0x100) break;
    }
}
}
// 1.13c D2Game+0x93D60
extern "C" void d2g_93d60(const D2G93D60Env* e, uint32_t seed, uint32_t game, uint32_t unit, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t step) {
    ring(*e, seed, game, unit, p3, p4, p5, p6, p7, step, e->fnAddr(0x91040u), true);
}
// 1.13c D2Game+0x94370: the same loop with callback 0x90EA0 and an unconditional #10637 call
extern "C" void d2g_94370(const D2G93D60Env* e, uint32_t seed, uint32_t game, uint32_t unit, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7, uint32_t step) {
    ring(*e, seed, game, unit, p3, p4, p5, p6, p7, step, e->fnAddr(0x90EA0u), false);
}
