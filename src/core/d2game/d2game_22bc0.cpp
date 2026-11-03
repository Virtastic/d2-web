// src/core/d2game/d2game_22bc0.cpp -- 1.13c D2Game.dll sub_22BC0 (player life/mana-style stat pair set), transcribed from the
// cached decompile and re-read against its disassembly (the decompile misses the +0x1c row field loads' row numbers).
// Convention as d2game_batch_aea40.cpp: raw retail records, register input explicit, callees weak externs.
#include <cstdint>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define S32(b, o) (*(int32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char sgptDataTables_exref[];
O(10237); O(10551); F(18A0);

// Field +0x1c of row `row` of the 0x20-byte table at dt+0xc50 (count dt+0xc54); 0 when the table is too short.
static inline S rowField(uintptr_t dt, S row)
{
    if (S32(dt, 0xc54) <= row) return 0;
    uintptr_t p = (uintptr_t)R32(dt, 0xc50) + (U)row * 0x20;
    return p ? S32(p, 0x1c) : 0;
}

// 1.13c D2Game+0x22BC0 -- ECX = unit (a player, type 1), stack = param (nonzero selects the second row pair). ret 4.
extern "C" void d2_fn_d2game_22BC0(I unit, S param)
{
    if (!unit || R32(unit, 0) != 1) return;
    I pl = R32(unit, 0x80);
    if (!pl) return;
    uintptr_t dt = (uintptr_t)R32(sgptDataTables_exref, 0);
    S a = rowField(dt, param ? 15 : 14);
    S b = rowField(dt, param ? 13 : 12);
    I row = Ord10237(R8(pl, 0x6d));
    if (row) {
        S m = S32(row, 0x34);
        a = (m * a) / 100;
        b = (m * b) / 100;
        S cls = (S)D2GAME_18A0(unit);
        if (cls >= 0 && cls < S32((uintptr_t)R32(sgptDataTables_exref, 0), 0xa80) && cls == 0x76) a /= 2;
    }
    Ord10551(unit, 0x19, a, 0);
    Ord10551(unit, 0x77, b, 0);
}
