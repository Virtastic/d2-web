// Batch 7A020: 12 D2Game functions from the cached 1.13c decompile. Register inputs
// (unaff_*/in_EAX/stack) are explicit parameters; every such argument is marked GUESS.
// Unmapped Ordinal_/FUN_ callees are reached through a hook table `H` (GUESS: names by ordinal).
#include <cstdint>

typedef int32_t i32; typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
struct U { i32 t; i32 f[0x60]; };  // unit view: [0]=type, [1]=id, [0x31]=flags (GUESS size)

struct H {  // hooks, GUESS: each is the Ordinal/FUN named in the field
    int (*o10648)(U*); int (*o10541)(U*); int (*o10830)(U*); int (*o10666)(U*, U*);
    int (*o10341)(U*); int (*o10239)(U*); int (*o10973)(U*, int, int); int (*o10913)(U*);
    int (*o11027)(U*); int (*f1900)(); int (*f18A0)(); int (*f1850)(); int (*f213b0)(int);
    bool (*f42a80)(int); void* (*o10045)(const char*); void (*o10158)();
    int (*dataTableMax)();  // *(sgptDataTables+0xa80)
    int (*bitmask12)();     // gdwBitMasks[0xc]
    int (*f21220)();        // returns record ptr as int
    int (*rec_d)(int);      // *(u8*)(rec+0xd)
};

static inline bool isUnit(U* a, U* b) { return a != b && a && a->t == 1; }
static inline bool hasF1(U* u) { return (u->f[0x30] >> 1) & 1; }   // param_3[0x31]>>1&1
static inline bool inRange(const H& h, int v) { return v >= 0 && v < h.dataTableMax(); }

struct Msg { u32 next; u8 type; u8 b9; u16 w; u32 a; u32 b; };
static void enqueue(const H& h, int* client, u32* m) {  // shared tail of 7A020/7A4C0
    if (client[0xec/4] == 0) client[0xec/4] = (int)(intptr_t)m;
    if (client[0xf0/4]) *(u32*)(intptr_t)client[0xf0/4] = (u32)(intptr_t)m;
    client[0xf0/4] = (int)(intptr_t)m;
    h.o10158();
}

// 1.13c D2Game+0x7A020   GUESS: client (ESI), arg (retaddr slot)
void D2GAME_7A020(const H& h, int* client, u32 arg) {
    u32* m = (u32*)h.o10045("..\\Source\\D2Game\\UNIT\\SUnitMsg.cpp");
    ((u8*)m)[0xe] = 0x2f; ((u8*)(m + 1))[0] = 0x23; m[0] = 0; ((u16*)(m + 3))[0] = 0; m[2] = arg;
    enqueue(h, client, m);
}

// 1.13c D2Game+0x7A4C0   GUESS: client (ESI), b11 (retaddr slot)
void D2GAME_7A4C0(const H& h, int* client, u32 p1, u8 p2, u8 b11) {
    u32* m = (u32*)h.o10045("..\\Source\\D2Game\\UNIT\\SUnitMsg.cpp");
    *(u16*)(m + 2) = 0x281; ((u8*)m)[10] = 0; ((u8*)m)[0xb] = b11; ((u8*)(m + 1))[0] = 0x99;
    m[0] = 0; m[3] = p1; ((u8*)(m + 4))[0] = p2;
    enqueue(h, client, m);
}

// 1.13c D2Game+0x7B510   GUESS: eax (game, +0x20 = 64-bit seed), esi (table base), unit
struct B510H { int (*o10331)(); int (*o10826)(int); int (*f1780)(void*); };
int D2GAME_7B510(const B510H& h, int game, int* esi) {
    int idx = h.o10826(h.o10331());
    int* e = (int*)(intptr_t)esi[0xf0/4 + idx];
    if (e[0x2d4/4] < 0) {
        if (idx < 0x28) { e[0x2d4/4] = 0; return 0; }
        e[0x2d4/4] = h.f1780((void*)(intptr_t)(game + 0x20));
    }
    return e[0x2d4/4];
}

// 1.13c D2Game+0x7B860   fastcall(ecx, a=param_2, b=param_3)
int D2GAME_7B860(const H& h, U* a, U* b) {
    if (b && (b->t == 1 || b->t == 0) && a != b && h.o10648(b) == 0) {
        int v = h.f1900();
        if (v <= b->f[0]) {  // param_3[1]
            int x = h.o10973(b, 6, 0), y = h.o10913(b);
            if (x <= (y / 100) * b->f[1]) { b->t = (int)(intptr_t)b; b->f[0] = v; }
        }
    }
    return 0;
}

// 1.13c D2Game+0x7B8E0   GUESS: out (puVar3 reuses the flags bit; ctx layout from decompile)
int D2GAME_7B8E0(const H& h, U* a, U* b, u32* c) {
    if (!(b && b->t == 1 && a != b && h.o10666(a, b) && h.o10830(b) != 2 && hasF1(b) && h.o10541(b) == 0)) return 0;
    int rec = h.f21220();
    if (c[5] == 0) { if (!(h.bitmask12() & h.rec_d(rec))) return 0; }
    else if (h.o10239(b) == 0) return 0;
    if ((c[6] != 0 || !h.f42a80(8)) && h.f1900() <= (int)c[4]) {
        c[3]++;
        if (b->f[3] == 0xc && h.o10341(b) == 0) { c[0] = (u32)(intptr_t)b; c[2]++; return 0; }
        int x = h.o10973(b, 6, 0), y = h.o10913(b);
        if (y != x && b->f[3] != 0) { c[1] = (u32)(intptr_t)b; c[2]++; }
    }
    return 0;
}

// 1.13c D2Game+0x7BA40   GUESS: type check 0x136 on FUN_6fc218a0 result (c = param_3 as ctx)
int D2GAME_7BA40(const H& h, U* a, U* b) {
    if (!(b != a && b && b->t == 1 && h.o10648(b) == 0 && hasF1(b))) return 0;
    int k = h.f18A0();
    if (!(inRange(h, k) && k == 0x136)) return 0;
    int v = h.f1900();
    if (v <= b->f[1] && h.o10666(a, b) && h.o10830(b) == 0) {
        if (v < b->f[2]) { b->t = (int)(intptr_t)b; b->f[2] = v; }
        if (v < b->f[3] && h.o11027(b) < 0x28) { b->f[0] = (int)(intptr_t)b; b->f[3] = v; }
    }
    return 0;
}

// 1.13c D2Game+0x7BB00   GUESS: ebx = out {best, dist}
int D2GAME_7BB00(const H& h, U* a, U* b, i32* ebx) {
    if (a != b && b && b->t == 1 && !((b->f[0x30] >> 9) & 1) && hasF1(b) && h.o10648(b) &&
        h.o10541(b) == 0 && h.o10830(b) == 0 && h.f213b0(0xc)) {
        int d = h.f1900();
        if (d <= ebx[1]) { ebx[0] = (i32)(intptr_t)b; ebx[1] = d; }
    }
    return 0;
}

// 1.13c D2Game+0x7BC70   GUESS: esi = out {best, count, bestVal, maxDist}
int D2GAME_7BC70(const H& h, U* a, U* b, i32* esi) {
    if (a != b && b && b->t == 1 && h.o10666(a, b) && h.o10830(b) != 2 && h.o10648(b) == 0) {
        h.f18A0();
        int k = h.f1850();
        if (k == 0xeb || k == 0xee) {
            if (h.f1900() <= esi[3]) {
                int v = h.o11027(b);
                if (v < 0x3d) {
                    if (esi[2] < 1) esi[2] = 0x7fffffff;
                    esi[1]++;
                    if (v < esi[2]) esi[0] = (i32)(intptr_t)b;
                }
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0x7BD00
int D2GAME_7BD00(const H& h, U* u) {
    if (u && u->t == 1 && h.o10648(u) == 0) { int k = h.f18A0(); return inRange(h, k) && k == 0x76; }
    return 0;
}

// 1.13c D2Game+0x7BD50   GUESS: ebx = ctx (+0xc maxdist), ecx = out {best,count,x}, edx = x
int D2GAME_7BD50(const H& h, U* a, U* b, i32* ctx, u32* out, u32 edx) {
    if (a != b && b && b->t == 1) {
        int k = h.f18A0();
        if (inRange(h, k) && (k == 0x13 || k == 0x3a) && h.o10830(b) == 0 && hasF1(b) &&
            h.o10541(b) == 0 && b->f[3] == 0xc && h.o10341(b) == 0 && h.f1900() <= ctx[3] && !h.f42a80(0xc)) {
            out[0] = (u32)(intptr_t)b; out[1]++; out[2] = edx;
        }
    }
    return 0;
}

// 1.13c D2Game+0x7BE10   GUESS: game (eax), unit (esi); hooks for ordinals 10750/10867/10870/10331/10671/11164, FUN_6fcae6f0, FUN_6fcaea80
struct BE10H { void (*o10750)(i32); void (*o10867)(i32); void (*o10870)(int); int (*o10331)();
               void (*o10671)(); void (*o11164)(i32, int); void (*fae6f0)(); void (*faea80)(int, i32*, int, int, int, int, int); };
void D2GAME_7BE10(const BE10H& h, int game, i32* u) {
    if (u) u[0x31] &= 0xfffffff1;
    auto live = [&] { return u[0] != 2 && (u[0] < 4 || u[0] > 5) && u[0xb] != 0; };
    if (live()) h.o10750(u[0xb]);
    if (live()) h.o10867(u[0xb]);
    h.o10870(h.o10331());
    if (live()) h.o10750(u[0xb]);
    if (live()) h.o10867(u[0xb]);
    h.o10870(h.o10331());
    h.o10671();
    h.o11164(u[0xb], 0);
    for (char* p = (char*)(intptr_t)u[0x37]; p;) { char c = *p; p = *(char**)(p + 0x24); if (c == 2) h.fae6f0(); }
    h.faea80(game, u, 2, *(int*)(intptr_t)(game + 0xa8) + 0xc, 0, 0, 0);
}

// 1.13c D2Game+0x7BF50   GUESS: ecx=game, edx=unit, a3=param_3; stack0xffffffd8 passed as `frame`
struct BF50H { void (*o11103)(int, int); void (*f21320)(); void (*f95e10)(int, void*); void (*fd122c0)(int); };
void D2GAME_7BF50(const BF50H& h, int game, int unit, int p3, void* frame) {
    int rec = *(int*)(intptr_t)(p3 + 0x1c);
    if (*(int*)(intptr_t)(p3 + 0x18) == 0) { h.fd122c0(0x28); return; }
    uint64_t s = (uint64_t)*(u32*)(intptr_t)(unit + 0x20) * 0x6ac690c5ull + *(u32*)(intptr_t)(unit + 0x24);
    *(uint64_t*)(intptr_t)(unit + 0x20) = s;
    u8 idx = *(u8*)(intptr_t)(game + 0x6d);
    if ((int)((u32)s % 100) < *(int16_t*)(intptr_t)(rec + 0x56 + idx * 2)) {
        h.o11103(unit, 0);
        if (unit) h.f21320();
        h.f95e10(game, frame);
        return;
    }
    h.fd122c0(*(int16_t*)(intptr_t)(rec + 0x5c + idx * 2));
}
