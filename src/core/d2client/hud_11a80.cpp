#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// 1.13c D2Client+0x11A80: HUD list scrollbar mouse state machine, stdcall(x, y).
// St mirrors D2Client+0x11C500..0x11C548 (globals 0x6FBCC500..); G is thumbY [0x6FBD33F4], H [0x6FBA9E18], flag [0x6FBA8BE8].
// Callees are supplied: hit(i) i=0..5 = 0x11110 0x110C0 0x11070 0x11020 0x10FD0 0x10F70 (each reads x, y), region = 0x11180,
// cell = 0x111F0 (y), flag = 0x111B0 (x), maxScroll = 0x11240, step = 0x11360, page = 0x117C0, tbl(sel) = *(int*)(*0x6FBA8B94 + sel*10).
struct D2C11A80St { int32_t D, count, T, B, s[7], gx, gy, P, sel, out, r540, r544; uint32_t last; };
struct D2C11A80G { int32_t thumbY, H, flag; };
struct D2C11A80Cb {
    int32_t (*hit)(int32_t i, int32_t x, int32_t y); int32_t (*region)(int32_t x, int32_t y); int32_t (*cell)(int32_t y);
    int32_t (*flag)(int32_t x); int32_t (*maxScroll)(); void (*step)(int32_t n); void (*page)(int32_t n);
    int32_t (*tbl)(int32_t sel); uint32_t (*tick)();
};
static_assert(sizeof(D2C11A80St) == 76 && offsetof(D2C11A80St, T) == 8 && offsetof(D2C11A80St, s) == 16 && offsetof(D2C11A80St, gx) == 44 &&
              offsetof(D2C11A80St, out) == 60 && offsetof(D2C11A80St, last) == 72, "11A80 state layout");

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_11a80(D2C11A80St* g, D2C11A80G* h, int32_t x, int32_t y, const D2C11A80Cb* cb) {
    g->out = 3;
    if (g->B != 0) return;
    int32_t hit[6];
    for (int32_t i = 0; i < 6; i++) hit[i] = cb->hit(i, x, y);
    const int32_t r = cb->region(x, y), u = r != 3;
    auto others = [&](int own) { for (int i = 0; i < 7; i++) if (i != own && g->s[i] != 0) return true; return false; };
    if (!others(-1) && !hit[0] && !hit[1] && !hit[2] && !hit[3] && !hit[4] && !hit[5] && !u) { g->B = g->T; return; }
    const int32_t T = g->T;
    const int32_t c = (int32_t)((uint32_t)h->H - 0x1A4u) / 2;
    auto tickGate = [&]() { if (!((uint32_t)g->last + 0x32u < cb->tick())) return false; g->last = cb->tick(); return true; };
    auto setY = [&](int32_t M) {   // ((0xF6 - 12D) * P) / M + 0x2F + c
        h->thumbY = (int32_t)((uint32_t)((int32_t)((uint32_t)(0xF6 - 12 * g->D) * (uint32_t)g->P) / M) + 0x2Fu + (uint32_t)c);
    };
    if (!others(0) && (hit[0] || g->s[0] != 0)) {      // A: track click
        g->s[0] = T;
        if (T != 0) {
            if (y < c + 0x28) cb->page(-1);
            if (c + 0x136 < y) cb->page(1);
            const int32_t os = g->sel, of = h->flag, sel = cb->cell(y);
            g->sel = sel; h->flag = cb->flag(x);
            if (cb->tbl(sel) == 0x39) { h->flag = of; g->sel = os; }
        }
    }
    if (!others(1) && hit[1]) {                          // up arrow
        g->s[1] = T;
        if (T != 0 && tickGate()) { const int32_t M = cb->maxScroll(); if (g->P > 0) g->P -= 1; setY(M); }
    }
    if (!others(2) && hit[2]) {                          // down arrow
        g->s[2] = T;
        if (T != 0 && tickGate()) { const int32_t M = cb->maxScroll(); if (g->P < M) g->P += 1; setY(M); }
    }
    if (!others(3) && hit[3]) { g->s[3] = T; if (T != 0 && tickGate()) cb->step(-14); }   // D
    if (!others(4) && (hit[4] || g->s[4] != 0)) {        // thumb drag
        if (hit[4] && g->s[4] == 0 && T != 0) { g->gx = h->thumbY; g->gy = y; }
        g->s[4] = T;
        if (T != 0) {
            const int32_t num = (int32_t)((uint32_t)((uint32_t)g->gx - (uint32_t)c - (uint32_t)g->gy - 0x2Fu + (uint32_t)y) * (uint32_t)(g->count + 1));
            cb->step(num / (0xF6 - 12 * g->D) - g->P);
        }
    }
    if (!others(5) && hit[5]) { g->s[5] = T; if (T != 0 && tickGate()) cb->step(14); }   // F
    if (!others(6) && u) { g->s[6] = T; g->out = cb->region(x, y); }
}
