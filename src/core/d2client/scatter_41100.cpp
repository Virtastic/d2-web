// 1.13c D2Client+0x41100 : scatter spawn. Decompile-derived (decompiler lost the register args; read from the
// disassembly: eax=unit, edx=seed low, stack args arg1..arg6 pushed by 0x41780 with arg1==edx).
#include <cstdint>
#include <cstring>
#include "../d2moo/include/D2Seed.h"
#include "../d2moo/include/Units/Units.h"

extern "C" int32_t d2_client_1400(int32_t room, int32_t x, int32_t y);    // room_find_1400.cpp
extern "C" void* d2client_msl_create_b30f0(const uint32_t* req);          // missile_create_b30f0.cpp

// 1.13c D2Client+0x41100: n = r*r tries; each draws two LCG (0x6AC690C5, carry word) offsets in [0,2r), dx=r-v1,
// dy=r-v2; inside the circle (dx^2+dy^2 <= n) and room-found -> create a type-1 record at (cx+dx, cy+dy).
// arg1 (the first pushed stack arg) is only a scratch slot overwritten by the room lookup; not a parameter here.
extern "C" void d2client_scatter_41100(void* unit, int32_t seedLow, int32_t cx, int32_t cy, int32_t r, int32_t a5, int32_t a6) {
    D2SeedStrc seed; SEED_InitLowSeed(&seed, seedLow);
    int32_t n = r * r;
    uint32_t rec[0x17] = {0};
    rec[1] = 1; rec[2] = (uint32_t)(uintptr_t)unit; rec[12] = (uint32_t)a5; rec[13] = (uint32_t)a6;
    int32_t room = (int32_t)(intptr_t)UNITS_GetRoom((D2UnitStrc*)unit);
    uint32_t m = (uint32_t)r * 2;
    auto draw = [&]() -> int32_t {
        if ((int32_t)m < 1) return 0;
        uint64_t p = (uint64_t)seed.nLowSeed * 0x6ac690c5u + seed.nHighSeed;
        seed.nLowSeed = (uint32_t)p; seed.nHighSeed = (uint32_t)(p >> 32);
        return (m & (m - 1)) == 0 ? (int32_t)((m - 1) & (uint32_t)p) : (int32_t)((uint32_t)p % m);
    };
    for (int32_t i = n; i > 0; --i) {
        int32_t dx = r - draw(), dy = r - draw();
        if (dx * dx + dy * dy > n) continue;
        rec[5] = (uint32_t)(dx + cx); rec[6] = (uint32_t)(dy + cy);
        if (d2_client_1400(room, dx + cx, dy + cy)) d2client_msl_create_b30f0(rec);
    }
}
