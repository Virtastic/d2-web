// 1.13c D2Client+0x40DF0 burst spawn
// Request block (disasm, locals base esp+0x18): [0]=3 [1]=p1 [4]=p6 [5]=p2 [6]=p3 [7]=x [8]=y [0xb]=p4 [0xc]=p5.
// #10614 SEED_InitLowSeed and #10513 SEED_GetLowSeed run for real (D2Common, ours); the spawn and the two setters are the environment.
#include <cstdint>
#include <cstring>
#include "../d2moo/include/D2Seed.h"
#include "d2client_40df0.h"

extern "C" void d2c_40df0(int32_t n, uint32_t seedLow, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4,
                          uint32_t p5, uint32_t p6, const D2c40df0Env* env) {
    D2SeedStrc seed;
    SEED_InitLowSeed(&seed, static_cast<int>(seedLow));
    uint32_t s[23] = {};
    s[0] = 3; s[1] = p1; s[5] = p2; s[6] = p3; s[4] = p6; s[11] = p4; s[12] = p5;
    for (; n > 0; --n) {
        uint64_t r; std::memcpy(&r, &seed, 8);
        uint64_t a = (r & 0xffffffff) * 0x6ac690c5u + (r >> 32);
        int32_t x = static_cast<int32_t>(static_cast<uint32_t>(a) % 0x28) - 0x14;
        r = (a & 0xffffffff) * 0x6ac690c5u + (a >> 32);
        int32_t y = static_cast<int32_t>(static_cast<uint32_t>(r) % 0x28) - 0x14;
        std::memcpy(&seed, &r, 8);
        if (x == 0 && y == 0) x = 0x14;
        s[7] = static_cast<uint32_t>(x); s[8] = static_cast<uint32_t>(y);
        void* m = env->create(s);
        if (m) {
            env->setX(m, static_cast<int>(SEED_GetLowSeed(&seed)));
            env->setY(m, y * 0x10000 + static_cast<int>(static_cast<uint32_t>(x) & 0xffff));
        }
    }
}
