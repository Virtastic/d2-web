// Implementation batch 5bea0: A5Q1 init and A5Q2 state check, from the cached 1.13c decompiles.
// Not yet linked or vectored (RE-DONE only). Callees by retail RVA (r_NNNN) / ordinal (oNNNNN).
#include <cstdint>
using i32 = int32_t; using u8 = uint8_t;
extern "C" {
void* o10045(const char*, i32, i32); i32 o10331(i32); i32 o10826(i32);
void r_587C0(); void r_5B380(); void r_5A720(); void r_5BDB0(); void r_5B4F0(); void r_59450();
void r_587A0(); void r_56E00(); void r_56D80(); void r_56D10(); extern char r_DAT_10ACB8[];
void r_9E170(void*);
}

// 1.13c D2Game+0x5BEA0
void D2GAME_5BEA0(i32 b)
{
    i32* z = (i32*)(intptr_t)(b + 0xa0);
    for (int i = 0; i < 0xf; i++) z[i] = 0;
    auto P = [&](int o, const void* f) { *(i32*)(intptr_t)(b + o) = (i32)(intptr_t)f; };
    *(u8*)(intptr_t)(b + 0xc) = 0;
    P(0xa0, (void*)r_587C0); P(0xc8, (void*)r_587A0); P(0xac, (void*)r_5B380); P(0xc0, (void*)r_5BDB0);
    P(0xa8, (void*)r_5A720); P(0xcc, (void*)r_5B4F0); P(0xd4, (void*)r_59450); P(0xdc, r_DAT_10ACB8);
    *(u8*)(intptr_t)(b + 10) = 1;
    i32* s = (i32*)o10045("..\\Source\\D2Game/Quests/a5q1.cpp", 0x2d3, 0);
    *(i32**)(intptr_t)(b + 0x18) = s;
    *(i32*)(intptr_t)(b + 0xe0) = 0x23; P(0xe8, (void*)r_56E00);
    *(u8*)(intptr_t)(b + 0xd) = 4;
    P(0xec, (void*)r_56D80); P(0xf0, (void*)r_56D10);
    *(i32*)(intptr_t)(b + 0x10) = 0x20;
    for (int i = 0; i < 6; i++) s[i] = 0;
}

// 1.13c D2Game+0x5DF70 (fastcall: ecx = event struct)
i32 D2GAME_5DF70(i32* p1)
{
    i32* l = (i32*)(intptr_t)(*(i32*)(intptr_t)p1[0] + 0x10f4);
    if (!*l) return 0;
    i32* q = (i32*)(intptr_t)**(i32**)l;
    for (; q; q = (i32*)(intptr_t)q[0x3d]) if (*q == 0x24) break;
    if (!q) return 0;
    i32 u = o10331(p1[2]);
    if (!u) return 0;
    if (o10826(u) == 0x83) {
        if (*(char*)(intptr_t)(q[6] + 0x86) == 1) r_9E170((void*)(intptr_t)p1[0]);
    } else r_9E170((void*)(intptr_t)p1[0]);
    return 0;
}
