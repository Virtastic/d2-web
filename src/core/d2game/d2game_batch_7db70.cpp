// 1.13c D2Game+0x7DB70 AITHINK_UseSkillOrCommandStep. Written from the 1.13c disassembly (objdump is authoritative:
// the decompile mixes up which argument is the unit and which is the task block's wait value).
// Register contract: ecx = unit U, edx = target T (may be null), one stdcall stack arg = task block B (ret 4).
// B[0] = state block S (S+0x14 is the wait counter), B[2] = value passed on to the callees, B[5] = N (first value
// compared to the row's per-mode gates), B[6] = F (target-present flag), B[7] = row R.
#include <cstdint>

namespace ordns_7db70 {
__attribute__((weak, import_module("env"), import_name("Ordinal_11027"))) int Ordinal_11027(void*);
__attribute__((weak, import_module("env"), import_name("Ordinal_10494"))) int Ordinal_10494(void*, int);
__attribute__((weak, import_module("env"), import_name("Ordinal_11103"))) void Ordinal_11103(void*, int);
__attribute__((weak, import_module("env"), import_name("FUN_6fc21180"))) uint32_t FUN_6fc21180(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc21320"))) void* FUN_6fc21320(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fc95e10"))) void FUN_6fc95e10(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fd12380"))) void FUN_6fd12380(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fd122c0"))) void FUN_6fd122c0(...);
__attribute__((weak, import_module("env"), import_name("FUN_6fd12540"))) void FUN_6fd12540(...);
}

// Callee seam: a probe replaces these pointers with retail-recorded results (default: the weak imports above).
namespace env_7db70 {
decltype(&ordns_7db70::Ordinal_11027) Ordinal_11027 = &ordns_7db70::Ordinal_11027;
decltype(&ordns_7db70::Ordinal_10494) Ordinal_10494 = &ordns_7db70::Ordinal_10494;
decltype(&ordns_7db70::Ordinal_11103) Ordinal_11103 = &ordns_7db70::Ordinal_11103;
decltype(&ordns_7db70::FUN_6fc21180) FUN_6fc21180 = &ordns_7db70::FUN_6fc21180;
decltype(&ordns_7db70::FUN_6fc21320) FUN_6fc21320 = &ordns_7db70::FUN_6fc21320;
decltype(&ordns_7db70::FUN_6fc95e10) FUN_6fc95e10 = &ordns_7db70::FUN_6fc95e10;
decltype(&ordns_7db70::FUN_6fd12380) FUN_6fd12380 = &ordns_7db70::FUN_6fd12380;
decltype(&ordns_7db70::FUN_6fd122c0) FUN_6fd122c0 = &ordns_7db70::FUN_6fd122c0;
decltype(&ordns_7db70::FUN_6fd12540) FUN_6fd12540 = &ordns_7db70::FUN_6fd12540;
}

namespace {
using namespace env_7db70;
inline uint8_t* at8(uintptr_t b, uint32_t o) { return reinterpret_cast<uint8_t*>(b + o); }
inline uint32_t* at32(uintptr_t b, uint32_t o) { return reinterpret_cast<uint32_t*>(b + o); }
inline int rowS16(uintptr_t row, uint32_t base, uint32_t mode) { return *reinterpret_cast<int16_t*>(row + base + mode * 2); }
// LCG step used inline by the original: seed' = lo*0x6ac690c5 + hi (64-bit), low word is the roll source.
inline uint32_t lcg(uint32_t* seed) {
    uint64_t v = static_cast<uint64_t>(seed[0]) * 0x6ac690c5ull + seed[1];
    seed[0] = static_cast<uint32_t>(v); seed[1] = static_cast<uint32_t>(v >> 32);
    return seed[0];
}
// Unit -> its +0x14 data block's +0x2C field, or 0 (the inline "type 1 with data" test).
inline uintptr_t unitAi(uintptr_t t) {
    if (t && *at32(t, 0) == 1 && *at32(t, 0x14)) return *at32(*at32(t, 0x14), 0x2C);
    return 0;
}
}

extern "C" int* sgptDataTables_exref;
extern "C" uint32_t gdwBitMasks_exref[];

// 1.13c D2Game+0x7DB70
extern "C" void D2GAME_AI_UseSkillOrCommandStep_7DB70(uintptr_t U, uintptr_t T, uint32_t* B)
{
    uintptr_t S = B[0], R = B[7];
    int N = static_cast<int>(B[5]);
    int F = static_cast<int>(B[6]);
    uint32_t P = B[2];
    uint32_t* seed = reinterpret_cast<uint32_t*>(T + 0x20);
    int dist = Ordinal_11027(reinterpret_cast<void*>(T));
    uint32_t mode = *at8(U, 0x6D);

    // Step 1: the row skill (R+0x172) is a valid skills.txt index whose +0x80 field is a valid state id; target in
    // range of the row's +0x56 value, state clear, and a roll under the +0x5C value: use the skill, done.
    int sk = *reinterpret_cast<int16_t*>(R + 0x172);
    if (sk >= 0) {
        uintptr_t dt = static_cast<uintptr_t>(static_cast<uint32_t>(*sgptDataTables_exref));
        if (sk < static_cast<int>(*at32(dt, 0xBA0))) {
            uintptr_t srec = *at32(dt, 0xB98) + static_cast<uint32_t>(sk) * 0x23C;
            int16_t st = *reinterpret_cast<int16_t*>(srec + 0x80);
            if (st >= 0 && Ordinal_10494(reinterpret_cast<void*>(T), st) == 0 && dist < rowS16(R, 0x56, mode) &&
                static_cast<int>(FUN_6fc21180(seed, 100)) < rowS16(R, 0x5C, mode)) {
                // ecx = T, al = byte R+0x181, stack (skill, 0, 0, 0): the original pushes three zeros here (P rides only in step 4)
                FUN_6fd12380(T, static_cast<int>(*reinterpret_cast<int16_t*>(R + 0x172)), 0, 0, 0, *at8(R, 0x181));
                return;
            }
        }
    }
    if (F != 0) {
        // Step 2: a roll mod 100 under +0x62 clears the pending order (11103) and posts a type-4 packet to T (value
        // 100, or 101 when the monster record's +0x104 byte has bit gdwBitMasks[4]); otherwise wait for +0x68 ticks.
        uint32_t r = lcg(seed) % 100;
        if (static_cast<int>(r) < rowS16(R, 0x62, mode)) {
            Ordinal_11103(reinterpret_cast<void*>(T), 0);
            uint32_t pkt[8] = {4, static_cast<uint32_t>(T), 0, 0, 0, 0, 0, 0};
            uint32_t val = 100;
            if (T) {
                uintptr_t rec = reinterpret_cast<uintptr_t>(FUN_6fc21320(*at32(T, 4)));
                if (rec && (*at8(rec, 0x104) & gdwBitMasks_exref[4])) val = 101;
            }
            pkt[2] = P;
            reinterpret_cast<uint8_t*>(pkt)[0x15] = static_cast<uint8_t>(val);
            FUN_6fc95e10(U, pkt, 1);
            return;
        }
        FUN_6fd122c0(U, rowS16(R, 0x68, mode));  // eax = U, stack = wait ticks
        return;
    }
    // Step 3 (no target): arm the counter when N is under the +0x6E gate and the counter is spent.
    int cnt = static_cast<int>(*at32(S, 0x14));
    if (N < rowS16(R, 0x6E, mode) && cnt <= 0) *at32(S, 0x14) = static_cast<uint32_t>(rowS16(R, 0x74, mode));
    // Step 4: command skill R+0x170 when the counter is zero and the target is not a low-state monster.
    int16_t sk2 = *reinterpret_cast<int16_t*>(R + 0x170);
    if (sk2 >= 0 && *at32(S, 0x14) == 0 &&
        !(T && *at32(T, 0) == 1 && *at32(T, 0x14) && *at8(*at32(T, 0x14), 0xE) >= 4)) {
        FUN_6fd12380(T, static_cast<int>(sk2), P, 0, 0, *at8(R, 0x180));
        *at32(S, 0x14) = static_cast<uint32_t>(rowS16(R, 0x74, mode));
        return;
    }
    if (static_cast<int>(*at32(S, 0x14)) > 0) *at32(S, 0x14) -= 1;
    uint32_t lo = lcg(seed);
    uint32_t r = lo % 100;  // the original divides the new low word; the stored seed low word is that same value
    if (static_cast<int>(r) < rowS16(R, 0x7A, mode)) {
        uint32_t v = lcg(seed);
        uintptr_t ai = unitAi(T);
        uint32_t m = (v & 1) + 6;
        *at32(ai, 0x18) = 2;
        if (m != 0) { if (m >= 0x4E) m = 0x4D; *at8(ai, 0x20) = static_cast<uint8_t>(m); }
        FUN_6fd12540(U, T, P, 2, 0, 0, 1, 7);
        return;
    }
    if (N < rowS16(R, 0x80, mode)) {
        uint32_t v = lcg(seed);
        uint32_t m = ((v & 0xFF) < 0x80) ? 5u : 6u;
        if (m == 1) m = 7;
        uintptr_t ai = unitAi(T);
        if (m != 0) *at32(ai, 0x18) = m;
        *at8(ai, 0x20) = 3;
        FUN_6fd12540(U, T, P, 2, 0, 0, 1, 0);
        return;
    }
    FUN_6fd122c0(U, 0xF);
}
