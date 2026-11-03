// src/core/d2game/d2game_b6da0.cpp -- 1.13c D2Game.dll AiBaalTargetDesirability (sub_B6DA0), transcribed from the cached decompile
// resolved against its disassembly (the decompile's locals are mislabelled; the frame slots below follow the assembly).
// Convention as d2game_batch_aea40.cpp: raw retail records, register inputs explicit, callees weak externs.
#include <cstdint>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define S32(b, o) (*(int32_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char sgptDataTables_exref[];
static inline uintptr_t DT() { return (uintptr_t)R32(sgptDataTables_exref, 0); }
static const char kAiBaalCpp[] = "..\\Source\\D2Game\\Ai\\AiBaal.cpp";   // DAT_6fd1b434
O(11138); O(10973); O(10828); O(10507); O(10306); O(10601); O(10494); O(11027); O(10839);
F(B0980);

static inline S stat(I u, U id) { return (S)Ord10973(u, id, 0); }
// 0x23C-byte skill row of Ord10601's index, or null when out of range (the null row is then read at +0x18E by retail).
static inline uint8_t rowByte(S idx) {
    uintptr_t row = 0;
    if (idx >= 0 && idx < S32(DT(), 0xba0)) row = (uintptr_t)(idx * 0x23c) + R32(DT(), 0xb98);
    return *(uint8_t*)(row + 0x18e);
}

// 1.13c D2Game+0xB6DA0 -- ECX = unit (the candidate), EAX = the attacker unit (null allowed), stack = param_1 (game). ret 4.
// Returns the weighted desirability / 22 (never 0 for a legal target), 0 when the unit is a player-class unit whose row byte +0x4E <= 1.
extern "C" S d2_fn_d2game_B6DA0(I unit, I attacker, I game)
{
    const I r11138 = Ord11138(game, unit, 0);
    S l1c = 0;
    if (attacker) {
        if ((S)D2GAME_B0980(R32(attacker, 0xc), R32(attacker, 0x10)) > 0x4b)
            if ((S)D2GAME_B0980(R32(attacker, 0xc), R32(attacker, 0x10)) < 0x4b) l1c = 100;
    }
    const S s24 = stat(unit, 0x24), s27 = stat(unit, 0x27), s29 = stat(unit, 0x29), s25 = stat(unit, 0x25), s2b = stat(unit, 0x2b);
    const S x15 = ((s2b + s29) + 4 * (s24 + 2 * s25) + s27) / 15;
    const S s16 = stat(unit, 0x16), s31 = stat(unit, 0x31), s33 = stat(unit, 0x33), s35 = stat(unit, 0x35), s37 = stat(unit, 0x37), s3a = stat(unit, 0x3a);
    const S l28 = (S)((s3a >> 8) + s37 + s35 + s33 + s31 + s16) / 2;
    const S edi = Ord10494(unit, 0xb) ? 100 : 0;
    const S l2c = (S)Ord11027(unit) >= 0x14 ? 0 : 100;
    S r1 = 0, byteA = 0, r2 = 0, byteB = 0;
    if (I t1 = Ord10828(unit)) {
        r1 = (S)Ord10306(unit, t1, 1);
        byteA = rowByte((S)Ord10601(t1, kAiBaalCpp, 0x75));
    }
    if (I t2 = Ord10507(unit)) {
        r2 = (S)Ord10306(unit, t2, 1);
        byteB = rowByte((S)Ord10601(t2, kAiBaalCpp, 0x7b));
    }
    const S A = byteB * r2 + byteA * r1;
    const S ebp = A / 4;
    S l10 = 0;
    if (r11138) l10 = 100;
    else if (!Ord10839(game, unit, 4)) l10 = 0x4b;
    if (unit && R32(unit, 0) == 1) {
        const S sid = S32(unit, 4);
        uintptr_t rec = 0;
        if (sid >= 0 && sid < S32(DT(), 0xa80)) rec = (uintptr_t)(sid * 0x1a8) + R32(DT(), 0xa78);
        if (!rec || *(uint8_t*)(rec + 0x4e) <= 1) return 0;
    }
    const S eax = l1c + l10;
    S v = (x15 + eax * 5 + 2 * (ebp + edi * 2 + l28) + l2c * 3) / 0x16;
    return v ? v : 1;
}
