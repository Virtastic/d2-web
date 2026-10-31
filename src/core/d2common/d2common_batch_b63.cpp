// Batch b63 (D2Common): plain implementations from the 1.13c decompile. Offsets are raw byte offsets;
// callees are declared extern by RVA name. Hidden register inputs are explicit leading parameters.
#include <cstdint>
#include <initializer_list>
#include <cstdlib>
extern "C" void d2_fn_d2common_e490(uint32_t* r);

typedef uint8_t* P;
template <class T> static inline T& F(void* p, int off) { return *(T*)((P)p + off); }
#define I(p, o) F<int32_t>(p, o)
#define U(p, o) F<uint32_t>(p, o)
// retail pointer fields are 32-bit: read/write 4 bytes, never pointer-width
#define PT(p, o) ((void*)(uintptr_t)F<uint32_t>(p, o))

extern "C++" {
void sub_4A9B0(void* ecx); void sub_45550(); void sub_AB50(void* unit); void sub_2E1F0(void* p);
void sub_4A670(); void sub_C940(void* unit);
uint8_t sub_222B0(); void sub_27180(void*, uint32_t, int, int); void sub_25C10(void*);
void q_d2common_batch_b63_Ordinal_10128(uint32_t, uint32_t, uint32_t); void Ordinal_10131(uint32_t); void Ordinal_10127(uint32_t);
void* sub_44BD0(int); void sub_44F90(void*); void sub_44950(); int sub_EEC0(int dir, void*, void*);
void sub_EB80(void*); void sub_E490(int); void* sub_F000(void*, int, int); void sub_10010(void* game, int a, int b);
void* sub_7D9B0(void* p, int id); void* sub_10730(void*); void* sub_10A00(); void sub_44F20();
void sub_106B0(uint32_t, const int32_t* tbl, void* ctx, uint32_t* out); void sub_2D3C0(int, int*, void*);
extern const int32_t DAT_6fde5f98[], DAT_6fde5fd8[], DAT_6fde6018[], DAT_6fde6058[], DAT_6fde6098[], DAT_6fde60d8[];
extern const int32_t DAT_6fde6ce8[]; // 6 dwords per entry
}

// 1.13c D2Common+0x7C0E0  (in_EAX = unit)
void sub_7C0E0(void* unit) {
    void* iVar1 = PT(PT(unit, 0x58), 0x1b4);
    if (I(unit, 0x2c) == 0) sub_45550();
    I(unit, 0x14) = I(unit, 4);
    I(unit, 0x18) = 0x29a;
    if (I(unit, 0x48) == 1) sub_4A670(); else if (I(unit, 0x48) == 2) sub_AB50(unit);
    if (I(unit, 0x48) == 1) sub_4A9B0(nullptr); else if (I(unit, 0x48) == 2) sub_C940(unit);
    U(unit, 0x28) |= 0x100000;
    sub_2E1F0(iVar1);
    F<char>(iVar1, 0x98) += 1;
    I(iVar1, 8) += 1;
}

// 1.13c D2Common+0x28D70  (fastcall: ecx=p1, edx=p2; unaff_retaddr=skip, unaff_EBX=ebx, unaff_EBP=ebp)
void sub_28D70(void* p1, uint32_t p2, int p3, int p4, int p5, int skip, int ebx, int ebp) {
    uint32_t a = 0;
    if (p1 && I(p1, 0) == 4 && PT(p1, 0x14)) a = U(PT(p1, 0x14), 0x18);
    uint32_t b = (a & 0xfff7ffff) | 0x800000;
    if (sub_222B0()) b = (a & 0xfff7ffff) | 0xa00000;
    if (p5) b = (b & 0xffbfffff) | 0x2000000;
    if (p3 == 0) { if (!(b & 0x10)) b &= 0xfffff7ff; }
    else q_d2common_batch_b63_Ordinal_10128(p2, 0x4d4a, 0x10);
    q_d2common_batch_b63_Ordinal_10128(p2, b, 0x20);
    if (!(b & 0x200000)) sub_27180(p1, p2, p3, skip); else sub_25C10(p1);
    if (skip == 0 && ebx != 0) {
        void* q = PT(p1, 0x60);
        if (q && I(q, 0) == 0x1020304 && (q = PT(q, 0xc))) {
            for (;;) {
                int t = I(q, 0);
                Ordinal_10131(p2);
                sub_28D70((t != 4) ? nullptr : q, p2, ebp, ebx, 0, 0, 0, 0);
                if (I(q, 0) != 4) break;
                int s = I(q, 0x14);
                if (s == 0 || s == -0x5c || !(q = PT((P)(intptr_t)s, 100))) break;
            }
        }
    }
    Ordinal_10127(p2);
}

// 0x44950 on blobs: add an orth node (room, dir) to owner's list head unless present.
static void link44950(void* owner, void* room, int dir) {
    for (void* n = PT(owner, 0); n; n = PT(n, 0x14))
        if (U(n, 0) == (uint32_t)(uintptr_t)room) return;
    uint32_t* nd = (uint32_t*)std::calloc(0x18, 1);
    uint32_t nn = (uint32_t)(uintptr_t)nd;
    nd[5] = U(owner, 0); U(owner, 0) = nn;
    nd[0] = (uint32_t)(uintptr_t)room; nd[1] = dir; nd[3] = 1; nd[4] = (uint32_t)(uintptr_t)room + 0x34;
}
static void pick(void* room) { uint32_t r[5] = {(uint32_t)(uintptr_t)room, 1, 0, 0, 0}; d2_fn_d2common_e490(r); }

// 1.13c D2Common+0x102F0  (in_EAX = ctx; param_1 = count). Pass k links prev->n with dirs
// (1,3),(0,2),(3,1),(2,0); EEC0 takes the pass direction in EAX.
void sub_102F0(void* ctx, int count) {
    void* first = PT(ctx, 0x10);
    void* cur = first;
    static const int dA[4] = {1, 0, 3, 2}, dB[4] = {3, 2, 1, 0};
    for (int pass = 0; pass < 4; pass++) {
        int n_steps = pass < 3 ? count - 1 : count - 2;
        for (int i = n_steps; i > 0; i--) {
            void* n = sub_44BD0(2);
            void* d = PT(PT(n, 0x16 * 4), 0x14);
            I(n, 0xf * 4) = I(d, 0x10); I(n, 0x10 * 4) = I(d, 0x14);
            if (sub_EEC0(dA[pass], cur, n) == 0) { sub_44F90(n); n = nullptr; }
            else {
                link44950(cur, n, dA[pass]); link44950(n, cur, dB[pass]); sub_EB80(n);
                void* e = PT(cur, 0x16 * 4);
                I(n, 9 * 4) = I(e, 0x10); F<uint32_t>(e, 0x10) = (uint32_t)(uintptr_t)n; I(e, 8) += 1;
                pick(cur); pick(n);
            }
            cur = n;
        }
    }
    link44950(cur, first, 2); link44950(first, cur, 0); pick(cur); pick(first);
}

// 1.13c D2Common+0x10AF0
void sub_10AF0(void* game) {
    void* r = PT(game, 0x10);
    uint64_t v = (uint64_t)U(r, 20) * 0x6ac690c5ull + U(r, 24);
    F<uint64_t>(r, 20) = v;
    uint32_t k = (uint32_t)v & 3;
    // (k*2 > 6 is unreachable after &3; retail _exit path omitted)
    const int32_t* e = DAT_6fde6ce8 + k * 6;
    int32_t a = e[0], b = e[2], c = e[4], d = e[1];
    void* n = sub_F000(r, e[1 - 0 + 0] * 0 + d, 0);
    if (n) { pick(r); void* q = PT(n, 0x20); I(q, 0) = a; I(q, 4) = b; I(q, 12) |= 2; }
    int32_t x = e[5], y = e[3];
    n = sub_F000(r, e[4], 0);
    if (n) { pick(r); void* q = PT(n, 0x20); I(q, 0) = y; I(q, 4) = x; I(q, 12) |= 2; }
    (void)c;
    sub_10010(game, 0x344, 0);
}

// 1.13c D2Common+0x10BE0  (unaff_ESI = ctx)
void sub_10BE0(void* ctx) {
    uint64_t v = (uint64_t)U(ctx, 0x1c4) * 0x6ac690c5ull + U(ctx, 0x1c8);
    F<uint64_t>(ctx, 0x1c4) = v;
    uint32_t k = (uint32_t)v & 3;
    sub_106B0(U(ctx, 0x1c8), DAT_6fde5f98 + k * 4, ctx, &k);
    sub_106B0(0, DAT_6fde5fd8 + k * 4, ctx, &k);
    sub_106B0(0, DAT_6fde6018 + k * 4, ctx, &k);
    int t = I(ctx, 0x1d0);
    if (t != 0x71) {
        if (t == 0x73) sub_106B0(0, DAT_6fde6058 + k * 4, ctx, &k);
        else if (t != 0x76) return;
    }
    sub_106B0(0, DAT_6fde6098 + k * 4, ctx, &k);
}

// 1.13c D2Common+0x10CA0
void sub_10CA0(void* g) {
    void* u = PT(g, 0x1b4);
    if (F<char>(u, 0x480) == 4) return;
    void* s = nullptr;
    for (void* p = PT(u, 0x47c); p; p = PT(p, 0x6b * 4)) if (I(p, 0x74 * 4) == 0x6c) { s = p; break; }
    if (!s) s = sub_7D9B0(u, 0x6c);
    void* n = sub_10730(g);
    struct S { int id, cnt; bool mark; };
    for (S x : {S{0x354, 3, true}, S{0x357, 1, true}, S{0x358, 1, false}, S{0x358, 1, false}}) {
        n = sub_F000(n, x.cnt, 0);
        if (n) {
            if (x.mark) sub_E490(1);
            void* q = PT(n, 0x20); I(q, 0) = x.id; I(q, 4) = -1; I(q, 12) |= 2;
        }
        if (x.id == 0x354) n = sub_10A00(); // retail fetches the next head after the first node
    }
    sub_44F20();
    int a = I(s, 7 * 4), b = I(n, 0xf * 4), c = I(s, 10 * 4), d = I(n, 0xe * 4), e = I(n, 0xd * 4), f = I(s, 8 * 4);
    uint64_t v = (uint64_t)U(g, 0x1c4) * 0x6ac690c5ull;
    v += U(g, 0x1c8);
    F<uint64_t>(g, 0x1c4) = v;
    sub_106B0((uint32_t)(v >> 32), DAT_6fde60d8 + ((uint32_t)v & 1) * 4, g, nullptr);
    sub_10010(g, 0x344, 0);  // retail passes the last placed room (ESI) as the 2nd stack arg: unresolved
    for (void* r = PT(g, 0x10); r; r = PT(r, 0x24)) {
        I(r, 0x34) += (b * 2 - e) + a;
        I(r, 0x38) += (c - d) + f;
    }
    int out = 0;
    sub_2D3C0(0, &out, g);
    I(g, 0x1c) = 0; I(g, 0x20) = 0; I(g, 0x24) = 0; I(g, 0x28) = out;
}
