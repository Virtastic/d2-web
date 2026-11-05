// Implementation batch 5a820: A5 quest (a5q2 / ice-quest-like) helpers, from the cached 1.13c decompiles.
// Callees by retail RVA (r_NNNN) / ordinal (oNNNNN). Not yet linked or vectored (RE-DONE only).
#include <cstdint>
#include <cstdlib>
using i32 = int32_t; using u32 = uint32_t; using u8 = uint8_t;
extern "C" {
intptr_t o10920(...); i32 o10174(i32, i32, i32); intptr_t o10024(...); intptr_t o10265(...);
intptr_t o10045(...);  /* ECX pool, EDX size, stack file, line, 0 (disassembly) */ void o10285(i32, int16_t);
intptr_t r_9BEB0(...); intptr_t r_9C160(...);
intptr_t r_E660(...); intptr_t r_DFCC0(...); /* EAX unit class (0 player), EBX callback RVA, EDI game, stack arg */ intptr_t r_EEF10(...); intptr_t r_33F80(...); /* EDI unit, ESI node, stack game */ void r_5C690(i32*, i32);
i32* r_47CF0(void* ecx, i32, i32, i32, i32);
// callbacks (RVA of function / label)
void r_58C90(); void r_57830(); void r_57D80(); void r_580A0(); void r_582E0(); void r_58640();
void r_583C0(); void r_582A0(); void r_5B1E0(); void r_5A310(); void r_5A530(); void r_5C9E0();
void r_581A0(); void r_58240(); void r_56C40(); void r_56BD0(); extern char r_DAT_2B2F0[];
}
extern i32** sgptDataTables_exref; extern i32 gdwBitMasks_exref;
static inline i32 sdt(i32 off) { return *(i32*)(*(i32*)sgptDataTables_exref + off); }
static inline i32 rd(i32 b, i32 o) { return *(i32*)(intptr_t)(b + o); }
static i32* findAI(i32* unit, i32 type) { // walk unit->0x10f4 list for entry of given type
    i32* l = (i32*)(intptr_t)unit[0x43d];
    if (!l) return nullptr;
    for (i32* p = (i32*)(intptr_t)*l; p; p = (i32*)(intptr_t)p[0x3d]) if (*p == type) return p;
    return nullptr;
}

// 1.13c D2Game+0x5A820: moved to d2game_5a820.cpp (env seam for the FN probe).

static i32 stateCb(i32 p1, i32* p2, i32 st, void* cb)
{
    i32 a = o10920(p2);
    i32 b = o10174(rd(a, 0x10 + *(u8*)(intptr_t)(p1 + 0x6d) * 4), st, 0xd);
    if (b && p2 && p2[0x20] && *p2 == 0 && (int16_t)r_9BEB0() != -1) r_9C160(cb);
    return 0;
}
// old form of D2Game 5AB00 (now d2game_5ab00.cpp)
i32 D2GAME_5AB00(i32 p1, i32* p2) { return stateCb(p1, p2, 0x28, (void*)r_58C90); }
// old form of D2Game 5AF50 (now d2game_5af50.cpp)
i32 D2GAME_5AF50(i32 p1, i32* p2) { return stateCb(p1, p2, 0x26, (void*)r_57830); }
// The quest-callback arms for the three Act V quests: moved to d2game_5b120.h, d2game_5b180.h and d2game_5b490.h.

// 1.13c D2Game+0x5B030 (p2 = in_EAX, REG? unit)
bool D2GAME_5B030(i32 p1, i32* in_EAX /* EAX = unit (5B8C0 passes its EDX) */)
{
    if (r_E660(0x20656369) != 0) return true;
    i32 st = rd(p1, 0x18);
    if (in_EAX && in_EAX[0x20] && *in_EAX == 0 && (int16_t)r_9BEB0() != -1) {
        *(u8*)(intptr_t)(st + 0xe2) = 0;
        r_9C160((void*)(intptr_t)r_57D80);
        return *(char*)(intptr_t)(st + 0xe2) != 0;
    }
    return false;
}

// 1.13c D2Game+0x5B8C0 (thiscall: this=ecx)
u32 D2GAME_5B8C0(void* self, i32* unitEdx, i32 /*a1, unused stack arg*/, i32 p2, u8* p3)
{
    i32 t = (i32)(intptr_t)self;
    *p3 = 0;
    if (o10174(p2, 0x25, 0) != 0) goto L1;
    if (o10174(p2, 0x25, 1) != 0) goto L1;
    if (o10174(p2, 0x25, 0xd) != 0) goto L1;
    if (*(char*)(intptr_t)(t + 9)) {
        if (D2GAME_5B030(t, unitEdx)) { *p3 = 4; return 1; }
        if (o10174(p2, 0x25, 0xe)) { *p3 = 0xc; return 1; }
        if (*(u8*)(intptr_t)(t + 0xc) < 5) { *p3 = *(u8*)(intptr_t)(t + 0xb); return 1; }
    }
    return 1;
L1:
    if (o10174(p2, rd(t, 0xe0), 8) && !o10174(p2, rd(t, 0xe0), 9)) { *p3 = 6; return 1; }
    if (!o10174(p2, rd(t, 0xe0), 8)) { *p3 = 5; return 1; }
    *p3 = 0xd;
    return 1;
}

// 1.13c D2Game+0x5C2A0: moved to d2game_5c2a0.cpp (env seam for the FN probe).

// The quest-node arm: moved to d2game_5cc30.h.

// 1.13c D2Game+0x5CC70
void D2GAME_5CC70(i32 b)
{
    i32* z = (i32*)(intptr_t)(b + 0xa0);
    for (int i = 0; i < 0xf; i++) z[i] = 0;
    auto P = [&](int o, void* f) { *(i32*)(intptr_t)(b + o) = (i32)(intptr_t)f; };
    *(u8*)(intptr_t)(b + 0xc) = 0;
    P(0xa0, (void*)r_583C0); P(0xc8, (void*)r_582A0); P(0xac, (void*)r_5B1E0); P(0xc0, (void*)r_5A310);
    P(0xa8, (void*)r_5A530); P(0xcc, (void*)r_5C9E0); P(0xd4, (void*)r_581A0); P(0xd8, (void*)r_58240);
    P(0xdc, (void*)r_DAT_2B2F0);
    *(u8*)(intptr_t)(b + 10) = 1;
    i32* s = (i32*)o10045(rd(rd(b, 4), 0x1c), 0x160, "..\\Source\\D2Game/Quests/a5q2.cpp", 0x323, 0);  // ECX = game pool, EDX = size
    *(i32**)(intptr_t)(b + 0x18) = s;
    *(i32*)(intptr_t)(b + 0xe0) = 0x24; *(i32*)(intptr_t)(b + 0xe8) = 0;
    *(u8*)(intptr_t)(b + 0xd) = 4;
    P(0xec, (void*)r_56C40); P(0xf0, (void*)r_56BD0);
    *(i32*)(intptr_t)(b + 0x10) = 0x21;
    for (int i = 0; i < 0x58; i++) s[i] = 0;
    *(int16_t*)&s[0x20] = 0;
}

// 1.13c D2Game+0x5CF70
void D2GAME_5CF70(i32* p1)
{
    i32* q = findAI((i32*)(intptr_t)p1[0], 0x21);
    if (!q) return;
    i32 st = q[6];
    if (rd(st, 0x88) == 1 && *(char*)(intptr_t)(st + 0x92) == 0) {
        o10285(2, (int16_t)sdt(0xb58));
        i32* r = r_47CF0((void*)(intptr_t)p1[2], p1[0], p1[5], p1[6], 1);
        if (r) { *(i32*)(intptr_t)(st + 0xa0) = r[3]; *(u8*)(intptr_t)(st + 0x92) = 1; }
    }
}
