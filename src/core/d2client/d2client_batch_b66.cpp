// D2Client batch b66-cli: env-seam style (globals/callees through one struct, each member names its address).
// Guest "pointers" are 32-bit addresses stored in 4-byte fields; A() widens them (same convention as batch_b59_cli_a).
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

namespace {
inline uint8_t* A(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t R32(uint32_t a, uint32_t off) { return *(uint32_t*)(A(a) + off); }
inline void W32(uint32_t a, uint32_t off, uint32_t v) { *(uint32_t*)(A(a) + off) = v; }
inline uint32_t sortKey(uint32_t ent) { return (R32(R32(ent, 0xc), 0x14) >> 14) & 7; }

// Depth-sorted insert shared by 79E20 / 7A340: slot = address of the 4-byte list head, e = new entry.
void insertSorted(uint32_t slot, uint32_t e) {
    uint32_t head = R32(slot, 0);
    if (head == 0) { W32(slot, 0, e); return; }
    uint32_t cur = head, prev = 0;
    for (;;) {
        uint32_t nx = R32(cur, 0x10);
        if (nx == 0) break;
        if (sortKey(e) < sortKey(cur)) {
            W32(e, 0x10, cur);
            if (prev == 0) W32(slot, 0, e); else W32(prev, 0x10, e);
            break;
        }
        prev = cur; cur = nx;
    }
    if (R32(cur, 0x10) == 0) W32(cur, 0x10, e);
}

// Common prologue: flag the cell, claim the next entry slot (3000 max). Returns entry address or 0.
uint32_t claimEntry(uint32_t grid, uint32_t* cell, uint32_t obj) {
    if (R32(obj, 0x1c) != 0 && R32(obj, 0x1c) != 0xd && (R32(obj, 0x14) & 4) == 0) *cell |= 4;
    uint32_t n = R32(grid, 0xea64);
    W32(grid, 0xea64, n + 1);
    if (n >= 3000) return 0;
    uint32_t e = grid + 4 + n * 0x14;
    W32(e, 0, 1); W32(e, 0xc, obj); W32(e, 0x10, 0);
    return e;
}
}  // namespace

struct D2cB66Env {
    void (*ord10561)(int32_t a, void* out32);   // Ordinal_10561(param, 32-byte out)
    int32_t (*ord10083)();                      // Ordinal_10083 (hidden register arg in 7BB40)
    int32_t (*ord10028)(uint32_t node);         // Ordinal_10028 (string measure; arg is node[0], decompile shows none)
    void (*ord10184)();
    void (*ord10014)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
    void (*ord10150)(int32_t, int32_t, int32_t);
    void (*ord10162)(int32_t, int32_t, int32_t, int32_t);
    void (*f6e4c0)();                           // D2Client+0x7E4C0
    void (*fatal)(int32_t code);                // Ordinal_10265+10024+_exit(-1); does not return
};

extern "C" {
// 1.13c D2Client+0x79E20
// this=ECX grid base; obj=EAX; ex=EBP, ey=EBX (hidden register inputs); cell=param_1; param_2 forwarded to Ordinal_10561.
// bit0 mask = gdwBitMasks[0] (== 1).
EMSCRIPTEN_KEEPALIVE void d2c_79e20(const D2cB66Env* env, uint32_t grid, uint32_t obj, int32_t ex, int32_t ey,
                                    uint32_t* cell, int32_t param_2) {
    uint32_t e = claimEntry(grid, cell, obj);
    if (!e) return;
    uint8_t buf[32];
    env->ord10561(param_2, buf);
    W32(e, 4, (uint32_t)((ex + (int32_t)R32(obj, 8) * 5) * 8));
    W32(e, 8, (uint32_t)((ey + (int32_t)R32(obj, 0xc) * 5) * 8));
    W32(grid, 0, R32(grid, 0) | 1u);
    insertSorted((uint32_t)(uintptr_t)cell + 0x20, e);
}

// 1.13c D2Client+0x79F20
// obj=EAX; ex=EDI, ey=ESI (hidden register inputs). Globals passed by value:
// g924f4=DAT_6fb924f4, g3b18/g3b1c/g3b20=DAT_6fbc3b18/1c/20.
EMSCRIPTEN_KEEPALIVE bool d2c_79f20(const D2cB66Env* env, uint32_t obj, int32_t ex, int32_t ey, int32_t param_1,
                                    int32_t g924f4, int32_t g3b18, int32_t g3b1c, int32_t g3b20) {
    if (g924f4 != 0) {
        uint32_t p = R32(obj, 0x10);
        if (p != 0 && (int32_t)R32(p, 0x28) != g3b1c) {
            if (g3b18 < (int32_t)R32(p, 0)) return true;
            return g3b20 < (int32_t)R32(p, 4);
        }
        return false;
    }
    uint8_t buf[32];
    env->ord10561(param_1, buf);
    int32_t x = ex + (int32_t)R32(obj, 8) * 5, y = ey + (int32_t)R32(obj, 0xc) * 5;
    uint32_t t = R32(obj, 0x1c);
    if (g3b18 * 5 < x && x - 0x14 < g3b18 * 5)
        for (uint32_t k : {1u, 4u, 5u, 7u, 8u, 10u, 12u}) if (t == k) return true;
    if (g3b20 * 5 < y && y - 0x14 < g3b20 * 5)
        for (uint32_t k : {2u, 3u, 6u, 7u, 9u, 11u, 12u}) if (t == k) return true;
    return false;
}

// 1.13c D2Client+0x7A340
// this=ECX grid; obj=EAX; ex=ESI, ey=EBX, edi=EDI (hidden register inputs; ex/edi forwarded to 79F20 as it expects them);
// tick = GetTickCount() result supplied by the platform layer. Other globals as in d2c_79f20.
EMSCRIPTEN_KEEPALIVE void d2c_7a340(const D2cB66Env* env, uint32_t grid, uint32_t obj, int32_t ex, int32_t ey, int32_t edi,
                                    uint32_t* cell, int32_t param_2, uint32_t tick,
                                    int32_t g924f4, int32_t g3b18, int32_t g3b1c, int32_t g3b20) {
    uint32_t e = claimEntry(grid, cell, obj);
    if (!e) return;
    int32_t t = (int32_t)(tick + 500);
    uint8_t* o = A(obj);
    if ((R32(obj, 0x14) & 4) == 0) {
        bool vis = d2c_79f20(env, obj, edi, ex, param_2, g924f4, g3b18, g3b1c, g3b20);
        uint32_t u = R32(obj, 0x24);
        if (!vis && (u & 1)) {
            o[0x29] = 0x80; o[0x2a] = 0xff;
            W32(obj, 0x2c, o[0x28] == 0x80 ? (uint32_t)t : (uint32_t)((int32_t)((0x80 - (uint32_t)o[0x28]) * 500) / 0x7f + t));
            W32(obj, 0x24, (u & ~1u) | 2);
        } else if (vis && !(u & 1)) {
            o[0x29] = 0xff; o[0x2a] = 0x80;
            W32(obj, 0x2c, o[0x28] == 0xff ? (uint32_t)t : (uint32_t)((int32_t)(((uint32_t)o[0x28] - 0xff) * 500) / 0x7f + t));
            W32(obj, 0x24, u | 3);
        }
    }
    uint8_t buf[32];
    env->ord10561(param_2, buf);
    W32(e, 4, (uint32_t)((ex + (int32_t)R32(obj, 8) * 5) * 8));
    W32(e, 8, (uint32_t)((ey + (int32_t)R32(obj, 0xc) * 5) * 8));
    uint32_t holder = *(uint32_t*)(buf + 28);   // iStack_4: dword 7 of the Ordinal_10561 output
    insertSorted(holder + 0x14, e);
}

// 1.13c D2Client+0x7BF60
// list=EDI (hidden register input): [0]=first node (node[0]=string, node[1]=next), [1]=count, +0x14 u16 min-width;
// writes [2],[3], u16 at +0x10 and +0x12.
EMSCRIPTEN_KEEPALIVE void d2c_7bf60(const D2cB66Env* env, uint32_t list) {
    int32_t widest = 0;
    W32(list, 8, 0);
    W32(list, 12, *(uint16_t*)(A(list) + 0x14));
    for (uint32_t n = R32(list, 0); n; n = R32(n, 4)) {
        int32_t w = env->ord10028(R32(n, 0));
        if (widest <= w) widest = w;
    }
    *(uint16_t*)(A(list) + 0x10) = (uint16_t)(widest + 0x14);
    *(uint16_t*)(A(list) + 0x12) = (uint16_t)((uint16_t)R32(list, 4) * 0xf + 6);
}

// 1.13c D2Client+0x7BB40
// tick = timeGetTime() from the platform layer. Globals by value/pointer: vis = (DAT_6fbc3ab6 || DAT_6fbc3aba),
// g3af6 = DAT_6fbc3af6, ec38 = DAT_6fbcec38, g91384 = DAT_6fb91384, *pbfa0 = DAT_6fbcbfa0.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_7bb40(const D2cB66Env* env, uint32_t p, uint32_t tick, bool vis, int32_t g3af6,
                                        int32_t ec38, int32_t g91384, int32_t* pbfa0) {
    if (p == 0) env->fatal(0x709);
    env->ord10184();
    if (R32(p, 0x1e) == 0) { W32(p, 0x1a, tick); W32(p, 0x1e, tick); }
    else {
        int32_t acc = 0;
        if (R32(p, 0x1a) == tick) {
            if (R32(p, 0x22) == 0) *(uint16_t*)(A(p) + 0x2a) += 1;
            else { acc = (int32_t)(R32(p, 0x26) + R32(p, 0x22)); W32(p, 0x26, (uint32_t)acc); }
        } else {
            W32(p, 0x26, 0);
            uint16_t c = *(uint16_t*)(A(p) + 0x2a);
            if (R32(p, 0x22) == 0 && c != 0) W32(p, 0x22, (tick - R32(p, 0x1a)) / c);
            W32(p, 0x1a, tick);
        }
        W32(p, 8, (uint32_t)((acc + (int32_t)((tick - R32(p, 0x1e)) >> 2)) * (int32_t)R32(p, 0xc)));
    }
    if (g3af6 == 0 && vis) {
        env->ord10014(ec38, g91384 - 5, ec38 + 0x145, g91384 + 0x75, 0, 1);
        env->f6e4c0();
    }
    uint32_t cur = R32(p, 0), first = 0, last = 0, lastT = 0, firstT = 0, sel = 0;
    uint32_t rem = R32(p, 8);
    bool done = false;
    if (rem != 0 && cur != 0) {
        do {
            if (rem < 0x17800) {
                if (vis) env->ord10150(g91384 + (0x70 - (int32_t)(rem >> 10)), 0, 0);
                sel = first; last = cur; lastT = rem;
                if (first == 0) { sel = cur; firstT = rem; first = cur; }
            }
            if (rem < 0x4800) break;
            cur = R32(cur, 4);
            rem -= 0x4800;
        } while (cur != 0);
        if (sel != 0 && R32(p, 0) != sel) {
            uint32_t prev = R32(p, 0), nx;
            do { nx = R32(prev, 4); if (nx == sel) break; prev = nx; } while (true);
            int32_t a = 0x5e - (int32_t)(firstT >> 10), b = a > 0x11 ? 0x12 : a;
            if (vis) env->ord10162(g91384 + a, 3, 0, b);
        }
        if (last != 0 && R32(last, 4) != 0) {
            int32_t a = -(int32_t)((lastT + 0x400) >> 10), b = a + 0x12, h = env->ord10083();
            if (b < h && vis) env->ord10162(g91384 + a + 0x83, 0, b, h - b);
        }
        done = first != 0 || last != 0;
    }
    if (!done) {
        uint32_t r = R32(p, 8);
        for (uint32_t n = R32(R32(p, 0), 4); n; n = R32(n, 4)) r -= 0x4800;
        if (r < 0x1c000) {
            int32_t a = 0x70 - (int32_t)((r + 0x400) >> 10);
            int32_t b = a < 0x12 ? a : 0x12;
            if (!(a < 0x12 && a == 0) && vis) env->ord10162(g91384 + a, 3, 0, b);
        }
    }
    env->ord10184();
    int32_t k = (int32_t)R32(p, 4) - 1;
    if (k < 1) k = 1;
    if ((uint32_t)(k * 0x12 + 0x70) < (R32(p, 8) >> 10)) {
        if (*pbfa0 == 1) *pbfa0 = 0;
        return 1;
    }
    return 0;
}
}
