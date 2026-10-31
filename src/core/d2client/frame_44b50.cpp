// D2Client+0x44B50: the per-frame pump (frame pacing, update, render, present). Callees go through D2cHost in retail order.
// Unported/import callees are tagged by RVA or ordinal; tags 0x97E4 = indirect [DAT_6fbc97e4](), 0xBF4C = vtbl call on [C+0xC] slot 3.
// 0x42400 leaves EDX untouched (disassembled), so the decompile's extraout_EDX is the v9808 it was given in EDX.
#include <emscripten.h>
#include <cstdint>
#include "client_host.h"
// Globals read/written, named by VA low bits (e.g. C394 = 0x6fbcc394).
struct D2c44B50G {
    uint32_t mode;      // C394
    uint32_t frameMs;   // BA4318
    uint32_t a1, a2;    // AADA4, AADAC
    uint32_t obj;       // CBBFC (nonzero == pointer)
    uint32_t last;      // C97F0
    uint32_t t97EC;     // C97EC
    uint32_t t97FC;     // C97FC
    uint32_t t9800;     // C9800
    uint32_t lim;       // C981C
    uint32_t skip;      // D3490
    uint32_t t9818;     // C9818
    uint32_t v9808, v9810, v980C, v9814, v97F4; // C9808 C9810 C980C C9814 C97F4
    uint32_t c3dc, c3e0, c3a0, c3a4, c3d4, c4d8; // CC3DC CC3E0 CC3A0 CC3A4 CC3D4 CC4D8
    uint32_t bf4c;      // CBF4C
    uint32_t h9920;     // C9920
};
// cl: a call with stack words s0,s1 (ordinals and 0x2B090/0xBE350 take stdcall stack args) and optional ECX/EDX.
static uint32_t cl(D2cHost h, uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t ecx = 0, uint32_t edx = 0) { D2cCall c = { rva, 0, ecx, edx, 0, 0, { s0, s1, 0, 0 }, { 0, 0, 0, 0 } }; return h(&c); }
// 1.13c D2Client+0x44B50
// now[0..1]: GetTickCount results in order (mainline: C97FC check, then C97EC; early path uses now[0]). Returns 0, or 1 on quit.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_44b50(D2cHost h, D2c44B50G* g, const uint32_t* now) {
    bool b1 = false, b2 = false;
    uint32_t pobj = g->obj;
    if (g->mode == 0 || g->mode == 1) {
        if (cl(h, 10043)) { g->last = now[0]; return 0; }
        if ((g->a1 || g->a2) && pobj && g->obj && cl(h, 10331, g->obj)) {
            g->last = now[0]; cl(h, 0x97E4); cl(h, 0x5F020); return 0;
        }
    }
    cl(h, 0xAB7D0);
    if (g->c3dc) cl(h, 0x44880);
    if (g->c3e0) cl(h, 0x446B0);
    cl(h, 0x43310);
    uint32_t t = now[0];
    if (g->t97FC < t) { cl(h, 0x14B00); g->t97FC = t + 5000; g->t9800 = t; }
    g->t97EC = now[1];
    if (g->last == 0) g->last = g->t97EC - g->frameMs;
    if (g->bf4c) { cl(h, 0xBF4C, g->last - g->t97EC); cl(h, 0x87840); }
    uint32_t fm = g->frameMs;
    cl(h, 0x423A0);
    uint32_t el = g->t97EC - g->last;
    if (fm <= el) {
        g->last = g->t97EC; b1 = true;
        if (g->t97EC < g->lim) {
            int32_t c = (int32_t)(fm - 1);
            if ((int32_t)(el - fm) < c) c = (int32_t)(el - fm);
            g->last = g->t97EC - c; g->skip = 0; g->t9818 = g->t97EC;
            g->v9808 = 0x19; g->v9810 = 0; g->v980C = 0; g->v9814 = 0; el = fm;
        }
        if (g->c3a0) cl(h, 0x2B090, pobj);
    }
    uint32_t r = 0;
    if (g->mode == 0) {
        cl(h, 10040); r = cl(h, 10008, 1);
        if (r) cl(h, 10024, 1, 0);
    } else if (g->mode == 1) {
        cl(h, 10040); r = cl(h, 10008, 1); cl(h, 10024, 0, 0);
    }
    cl(h, 0x43920); cl(h, 0x6FCE0); cl(h, 10050); cl(h, 0x435D0);
    if (g->c3a0) {
        if (g->mode == 0 || g->mode == 1) {
            if (r) {
                cl(h, 0x44A80); b2 = true;
                if (g->frameMs < el) {
                    if (el < g->frameMs * 2) { g->last += g->frameMs - el; g->skip = 0; }
                    else { g->skip = (int32_t)g->skip < 2; g->last += 1 - g->frameMs; }
                } else g->skip = 0;
            }
        } else if (b1) {
            cl(h, 0x44A80); g->last += g->frameMs - el; b2 = true;
            D2cCall c = { 0x42400, 0, el / g->frameMs, g->v9808, 0, 0, { 0, 0, 0, 0 }, { g->v9808, 0, 0, 0 } };
            int32_t v = (int32_t)h(&c);
            if (0x1c < c.out[0]) v = 0;  // EDX after the call: 0x42400 leaves it as passed (v9808)
            if (v < (int32_t)g->skip) g->skip = 0;
        } else if (0x16 < g->v9808) g->skip = 0;
        if (g->c3a0) {
            g->c4d8 = 0; cl(h, 0x51E80);
            if (cl(h, 10057)) { cl(h, 10023, 0, 0, 0xb4); cl(h, 10014); cl(h, 10061, 0, 0, 0xff); }
            if (g->skip == 0) {
                if (g->obj && cl(h, 10331, g->obj)) { cl(h, 0x97E4); g->v97F4++; g->v980C++; }
            } else if (b1) g->v9814++;
            if (b2) cl(h, 0x5F020);
            g->skip++;
            goto tail;
        }
    }
    cl(h, 10043);
tail:
    if (!g->c3a4) return 0;
    cl(h, 0x874E0); cl(h, 10065); cl(h, 0xBE350, 1, 0);
    if (g->mode == 0 || g->mode == 1) cl(h, 10006);
    if (g->mode == 6 || g->mode == 8) { cl(h, 0xC97C8); g->c3d4 = 1; cl(h, 0xC97C9); cl(h, 0xC9920, g->h9920, 10000); g->h9920 = 0; }
    return 1;
}
