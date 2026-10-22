// Implementation batch 60080 (b48-d2game-g2): A5Q6 init, damage-roll helper, 0x734E0 mode dispatch,
// 0x752A0 table dispatch, from the cached 1.13c decompiles. RE-DONE only, not vectored.
// Retail callees are weak seams (r_<RVA>) / D2Common ordinals (o<N>); register-only inputs are explicit params.
// Skipped: 674A0 6A330 6A720 6C900 6CF90 70FB0 711D0 717E0 (register-carried inputs not recoverable).
#include <cstdint>
using i32 = int32_t; using u8 = uint8_t;
#define W __attribute__((weak))
extern "C" {
W void* o10045(const char*, i32, i32); W i32 o10621(i32*); W i32 o10265(i32); W void o10024(const char*, i32);
W i32 o10061(i32); W i32 o11088(i32); W i32 o10804(i32*, i32, i32); W i32 o10910(i32, i32, i32); W i32 o10858();
W void r_77000(); W void r_76FC0(); W void r_7A9B0(); W void r_7FE30(); W void r_7D920(); W void r_77270();
W void r_78930(); W void r_76EB0(); W void r_76EF0(); W void r_79930(); W char r_DAT_10D1C0[1];
W void r_92FC0(i32*, i32*); W void r_D4570(); W i32* r_749A0(i32*); W void r_750F0();
W extern i32* sgptDataTables;
}

// 1.13c D2Game+0x60080 (fastcall: ecx = quest record)
void D2GAME_60080(i32 b)
{
    i32* z = (i32*)(intptr_t)(b + 0xa0);
    for (int i = 0; i < 0xf; i++) z[i] = 0;
    auto P = [&](int o, const void* f) { *(i32*)(intptr_t)(b + o) = (i32)(intptr_t)f; };
    *(u8*)(intptr_t)(b + 0xc) = 0;
    P(0xa0, (void*)r_77000); P(0xc8, (void*)r_76FC0); P(0xac, (void*)r_7A9B0); P(0xc0, (void*)r_7FE30);
    P(0xa8, (void*)r_7D920); P(0xcc, (void*)r_77270); P(0xd4, (void*)r_78930); P(0xd8, (void*)r_76EB0);
    P(0xdc, r_DAT_10D1C0);
    *(u8*)(intptr_t)(b + 10) = 1;
    i32* s = (i32*)o10045("..\\Source\\D2Game/Quests/a5q6.cpp", 0x329, 0);
    *(i32**)(intptr_t)(b + 0x18) = s;
    *(i32*)(intptr_t)(b + 0xe0) = 0x28; *(i32*)(intptr_t)(b + 0xe8) = 0;
    *(u8*)(intptr_t)(b + 0xd) = 4;
    P(0xec, (void*)r_76EF0); P(0xf0, (void*)r_79930);
    *(i32*)(intptr_t)(b + 0x10) = 0x25;
    for (int i = 0; i < 0x2a; i++) s[i] = 0;
    *(int16_t*)&s[0x20] = 0;
    s[0x25] = 1; s[0x27] = 1;
}

// 1.13c D2Game+0x69B00 (fastcall: ecx = param_1, edx = unit, stack = out; eax = in_EAX = second unit/arg)
void D2GAME_69B00(i32 p1, i32* unit, i32* out, i32 eax)
{
    *out = o10621(unit);
    if (p1 == 0) { o10024("", o10265(0x963)); __builtin_trap(); }
    if (eax == 0) eax = o10061(unit[0x18]);
    if (eax != 0 && o11088(eax) == 0x26) *out = 0;
    i32 a = o10804(unit, eax, p1);
    i32 b = o10910((i32)(intptr_t)unit, 0x77, 0);
    i32* t = (i32*)(intptr_t)o10858();
    i32 v = *out + (((0 - 1) * t[0x19c / 4] + t[0x198 / 4] + a + b) * *out) / 100;
    *out = v;
    if (unit && unit[0] == 0 && unit[1] >= 0) {
        i32* dt = (i32*)(intptr_t)*(i32*)sgptDataTables;
        if (unit[1] < dt[0xbc8 / 4]) {
            i32 row = unit[1] * 0xc4 + dt[0xbc4 / 4];
            if (row) *out = *(i32*)(intptr_t)(row + 0x3c) + v;
        }
    }
}

// 1.13c D2Game+0x734E0 (thiscall: cx = mode, edx = param_2, eax = unit)
void D2GAME_734E0(int16_t mode, i32* param_2, i32* u)
{
    if (mode == 0x17) {
        if ((char)u[0x1b] != 0 && u[0x1a] == 4) { u[0x19] = -1; u[0x1a] = 6; *(u8*)&u[0x1b] = 0; }
        r_D4570();
    } else if (mode == 0x18) r_92FC0(param_2, u);
}

// 1.13c D2Game+0x752A0 (fastcall: edx = param_2)
void D2GAME_752A0(i32, i32* param_2)
{
    i32* e = r_749A0(param_2);
    if (e && e[2]) ((void (*)())(intptr_t)e[2])();
    else r_750F0();
}
