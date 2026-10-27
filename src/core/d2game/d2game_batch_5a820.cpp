// Implementation batch 5a820: A5 quest (a5q2 / ice-quest-like) helpers, from the cached 1.13c decompiles.
// Callees by retail RVA (r_NNNN) / ordinal (oNNNNN). Not yet linked or vectored (RE-DONE only).
#include <cstdint>
#include <cstdlib>
using i32 = int32_t; using u32 = uint32_t; using u8 = uint8_t;
extern "C" {
i32 o10920(i32*); i32 o10174(i32, i32, i32); void o10024(const void*, i32); i32 o10265(i32);
void* o10045(const char*, i32, i32); void o10285(i32, int16_t);
int16_t r_9BEB0(); void r_9C160(void*);
i32 r_E660(u32); void r_DFCC0(i32*, i32); i32* r_EEF10(i32*); void r_33F80(void* pUnitEdi, void* pNodeEsi); void r_5C690(i32*, i32);
i32* r_47CF0(void*, i32, i32, i32);
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

// 1.13c D2Game+0x5A820
void D2GAME_5A820(i32* p1)
{
    i32* e = (i32*)(intptr_t)(*(i32*)(intptr_t)p1[0] + 0x10f4);
    i32* q = nullptr;
    if (*e) for (i32* p = (i32*)(intptr_t)**(i32**)e; p; p = (i32*)(intptr_t)p[0x3d]) if (*p == 0x24) { q = p; break; }
    i32 st = q[6];
    if (*(char*)(intptr_t)(st + 0xa0) != 0) return;
    i32 n = sdt(0xa80), loc = 0;
    do {
        if (loc > 9) break;
        i32* rng = (i32*)(intptr_t)rd(q[1], 0x10f4);
        if (!rng) { o10024((void*)0, o10265(0x1fb)); std::_Exit(-1); }
        i32 v;
        if (n < 2) v = 1;
        else {
            uint64_t m = (uint64_t)(u32)rng[6] * 0x6ac690c5ULL + (u32)rng[7];
            *(uint64_t*)&rng[6] = m;
            u32 r = ((n - 1) & (n - 2)) == 0 ? (u32)(n - 2) & (u32)m : (u32)((m & 0xffffffff) % (u32)(n - 1));
            v = r + 1;
        }
        loc++;
        *(i32*)(intptr_t)(st + 0xa4) = v;
        if (v >= 0 && v < sdt(0xa80)) {
            i32 rec = v * 0x1a8 + sdt(0xa78);
            if (rec && ((u32)*(u8*)(intptr_t)(rec + 0xe) & *(u32*)(intptr_t)(gdwBitMasks_exref + 0x18))) *(u8*)(intptr_t)(st + 0xa0) = 1;
        }
    } while (*(char*)(intptr_t)(st + 0xa0) == 0);
    if (*(char*)(intptr_t)(st + 0xa0) == 0) { *(u8*)(intptr_t)(st + 0xa0) = 1; *(i32*)(intptr_t)(st + 0xa4) = 0; }
    r_DFCC0(p1, 0);
}

static i32 stateCb(i32 p1, i32* p2, i32 st, void* cb)
{
    i32 a = o10920(p2);
    i32 b = o10174(rd(a, 0x10 + *(u8*)(intptr_t)(p1 + 0x6d) * 4), st, 0xd);
    if (b && p2 && p2[0x20] && *p2 == 0 && r_9BEB0() != -1) r_9C160(cb);
    return 0;
}
// 1.13c D2Game+0x5AB00
i32 D2GAME_5AB00(i32 p1, i32* p2) { return stateCb(p1, p2, 0x28, (void*)r_58C90); }
// 1.13c D2Game+0x5AF50
i32 D2GAME_5AF50(i32 p1, i32* p2) { return stateCb(p1, p2, 0x26, (void*)r_57830); }
// 1.13c D2Game+0x5B120
i32 D2GAME_5B120(i32 p1, i32* p2) { return stateCb(p1, p2, 0x25, (void*)r_580A0); }
// 1.13c D2Game+0x5B180
i32 D2GAME_5B180(i32 p1, i32* p2) { return stateCb(p1, p2, 0x24, (void*)r_582E0); }
// 1.13c D2Game+0x5B490
i32 D2GAME_5B490(i32 p1, i32* p2) { return stateCb(p1, p2, 0x23, (void*)r_58640); }

// 1.13c D2Game+0x5B030 (p2 = in_EAX, REG? unit)
bool D2GAME_5B030(i32 p1, i32* in_EAX)
{
    if (r_E660(0x20656369) != 0) return true;
    i32 st = rd(p1, 0x18);
    if (in_EAX && in_EAX[0x20] && *in_EAX == 0 && r_9BEB0() != -1) {
        *(u8*)(intptr_t)(st + 0xe2) = 0;
        r_9C160((void*)(intptr_t)r_57D80);
        return *(char*)(intptr_t)(st + 0xe2) != 0;
    }
    return false;
}

// 1.13c D2Game+0x5B8C0 (thiscall: this=ecx)
u32 D2GAME_5B8C0(void* self, i32 /*p1*/, i32 p2, u8* p3)
{
    i32 t = (i32)(intptr_t)self;
    *p3 = 0;
    if (o10174(p2, 0x25, 0) != 0) goto L1;
    if (o10174(p2, 0x25, 1) != 0) goto L1;
    if (o10174(p2, 0x25, 0xd) != 0) goto L1;
    if (*(char*)(intptr_t)(t + 9)) {
        if (D2GAME_5B030(t, nullptr /*REG? in_EAX*/)) { *p3 = 4; return 1; }
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

// 1.13c D2Game+0x5C2A0
i32 D2GAME_5C2A0(i32 q, i32 p2, i32 p3)
{
    i32 n = 0;
    if (q && *(char*)(intptr_t)(q + 9)) {
        for (i32 k = 0; k < 0x19; k++) {
            i32 a[10] = { rd(q, 4), rd(p2, 8), 0, 0x216, 1, 0, rd(p2, 0x14), rd(p2, 0x18), 5, 0 };
            i32* u = r_EEF10(a);
            if (u) {
                u[0x31] |= 0x3000000;
                *(i32*)(intptr_t)(rd(q, 0x18) + 0xb0 + n * 4 + p3 * 0x14) = u[3];
                n++;
                i32* l = (i32*)(intptr_t)rd(rd(q, 4), 0x10f4);
                if (l) for (i32* p = (i32*)(intptr_t)*l; p; p = (i32*)(intptr_t)p[0x3d]) if (*p == 0x20) { r_33F80((void*)(intptr_t)q, p); break; }
                if (n > 4) return n;
            }
        }
    }
    return n;
}

// 1.13c D2Game+0x5CC30
void D2GAME_5CC30(i32 unit)
{
    i32* q = findAI((i32*)(intptr_t)unit, 0x21); // list lives at unit+0x10f4
    if (!q) return;
    i32 st = q[6];
    *(u8*)(intptr_t)(st + 0xad) = 1;
    *(i32*)(intptr_t)(st + 0x84) = 2;
    r_5C690(q, st);
}

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
    i32* s = (i32*)o10045("..\\Source\\D2Game/Quests/a5q2.cpp", 0x323, 0);
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
        i32* r = r_47CF0((void*)(intptr_t)p1[2], p1[0], p1[5], p1[6]);
        if (r) { *(i32*)(intptr_t)(st + 0xa0) = r[3]; *(u8*)(intptr_t)(st + 0x92) = 1; }
    }
}
