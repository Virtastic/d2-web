// src/core/d2game/d2game_b6da0.cpp -- 1.13c D2Game.dll AiBaalTargetDesirability (sub_B6DA0), transcribed from the cached decompile
// resolved against its disassembly (the decompile's locals are mislabelled; the frame slots below follow the assembly).
// Callees go through the ord/sub seams of D2GameBatchB6DA0Env (ord = D2Common ordinal, sub = D2Game RVA), arguments in retail stack order.
#include <cstdint>
#include <emscripten.h>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define S32(b, o) (*(int32_t*)((uintptr_t)(b) + (o)))

struct D2GameBatchB6DA0Env {
    S (*ord)(U n, U a, U b, U c, U d);
    S (*sub)(U rva, U a, U b, U c, U d, U e, U f, U g);
    const uint8_t* dt;      // *sgptDataTables_exref
};
static const D2GameBatchB6DA0Env* G;
static inline uintptr_t DT() { return (uintptr_t)G->dt; }
static const char kAiBaalCpp[] = "..\\Source\\D2Game\\Ai\\AiBaal.cpp";   // DAT_6fd1b434 (RVA 0xFB434)
static inline S o(U n, I a = 0, I b = 0, I c = 0, I d = 0) { return G->ord(n, (U)a, (U)b, (U)c, (U)d); }
static inline S stat(I u, U id) { return (S)o(10973, u, id, 0); }
// 0x23C-byte skill row of Ord10601's index, or null when out of range (the null row is then read at +0x18E by retail).
static inline uint8_t rowByte(S idx) {
    uintptr_t row = 0;
    if (idx >= 0 && idx < S32(DT(), 0xba0)) row = (uintptr_t)(idx * 0x23c) + R32(DT(), 0xb98);
    return *(uint8_t*)(row + 0x18e);
}

// 1.13c D2Game+0xB6DA0 -- ECX = unit (the candidate), EAX = the attacker unit (null allowed), stack = param_1 (game). ret 4.
// Returns the weighted desirability / 22 (never 0 for a legal target), 0 when the unit is a player-class unit whose row byte +0x4E <= 1.
extern "C" EMSCRIPTEN_KEEPALIVE S d2_fn_d2game_B6DA0(I unit, I attacker, I game, const D2GameBatchB6DA0Env* env)
{
    G = env;
    const I r11138 = o(11138, game, unit, 0);
    S l1c = 0;
    if (attacker) {
        if ((S)G->sub(0xB0980, R32(attacker, 0xc), R32(attacker, 0x10), 0, 0, 0, 0, 0) > 0x4b)
            if ((S)G->sub(0xB0980, R32(attacker, 0xc), R32(attacker, 0x10), 0, 0, 0, 0, 0) < 0x4b) l1c = 100;
    }
    const S s24 = stat(unit, 0x24), s27 = stat(unit, 0x27), s29 = stat(unit, 0x29), s25 = stat(unit, 0x25), s2b = stat(unit, 0x2b);
    const S x15 = ((s2b + s29) + 4 * (s24 + 2 * s25) + s27) / 15;
    const S s16 = stat(unit, 0x16), s31 = stat(unit, 0x31), s33 = stat(unit, 0x33), s35 = stat(unit, 0x35), s37 = stat(unit, 0x37), s3a = stat(unit, 0x3a);
    const S l28 = (S)((s3a >> 8) + s37 + s35 + s33 + s31 + s16) / 2;
    const S edi = o(10494, unit, 0xb) ? 100 : 0;
    const S l2c = (S)o(11027, unit) >= 0x14 ? 0 : 100;
    S r1 = 0, byteA = 0, r2 = 0, byteB = 0;
    if (I t1 = o(10828, unit)) {
        r1 = (S)o(10306, unit, t1, 1);
        byteA = rowByte((S)o(10601, t1, (I)(uintptr_t)kAiBaalCpp, 0x75));
    }
    if (I t2 = o(10507, unit)) {
        r2 = (S)o(10306, unit, t2, 1);
        byteB = rowByte((S)o(10601, t2, (I)(uintptr_t)kAiBaalCpp, 0x7b));
    }
    const S A = byteB * r2 + byteA * r1;
    const S ebp = A / 4;
    S l10 = 0;
    if (r11138) l10 = 100;
    else if (!o(10839, game, unit, 4)) l10 = 0x4b;
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
