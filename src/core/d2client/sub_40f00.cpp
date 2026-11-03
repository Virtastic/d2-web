// D2Client sub_40F00 (FUN_6faf0f00): real function over the retail tables and sgptDataTables.
#include <cstdint>
#include <cstring>
#include "../d2moo/include/D2Seed.h"
#define W __attribute__((weak))
extern "C" {
W uint8_t* sgptDataTables;
W uint32_t DAT_6fb862e0[64], DAT_6fb863e0[64];
W void Ordinal_10637(void* missile, int32_t level);
void* d2client_msl_create_b30f0(const uint32_t* req);   // D2Client+0xB30F0 (missile_create_b30f0.cpp)

// 1.13c D2Client+0x40F00
// unaff_EBX = skill id (hidden register); param_5 unused by retail. Request words: [0]=3 [1]=p1 [4]=p2 [5]=p3 [6]=p4 [7]=ty [8]=tx.
void FUN_6faf0f00(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                  uint32_t param_5, int32_t param_6, int32_t unaff_EBX, uint32_t unaff_EDX) {
    D2SeedStrc seed; SEED_InitLowSeed(&seed, static_cast<int>(unaff_EDX));   // #10614 (ecx=&seed, edx=unit+0x28)
    uint32_t s[0x17]; memset(s, 0, sizeof s);
    s[0] = 3; s[1] = param_1; s[4] = param_2; s[5] = param_3; s[6] = param_4;
    s[0xb] = static_cast<uint32_t>(unaff_EBX); s[0xc] = param_5; s[0x15] = 0x6faf0200u;   // disasm: +0x40 skill id, +0x44 param_5, +0x68 callback
    for (int32_t i = 0; i < 0x100; i += param_6 * 4) {
        s[8] = *(const uint32_t*)((const uint8_t*)DAT_6fb862e0 + i);
        s[7] = *(const uint32_t*)((const uint8_t*)DAT_6fb863e0 + i);
        { uint64_t r; std::memcpy(&r, &seed, 8); r = (r & 0xffffffff) * 0x6ac690c5u + (r >> 32); std::memcpy(&seed, &r, 8); s[0x16] = static_cast<uint32_t>(r); }
        void* m = d2client_msl_create_b30f0(s);
        int32_t lvl = 0;
        if (unaff_EBX >= 0 && unaff_EBX < *(const int32_t*)(sgptDataTables + 0xba0)) {
            const uint8_t* r = *(uint8_t* const*)(sgptDataTables + 0xb98) + (uint32_t)unaff_EBX * 0x23c;
            if (r) lvl = *(const int32_t*)(r + 0x14c);
        }
        Ordinal_10637(m, lvl + 1);
    }
}
}
