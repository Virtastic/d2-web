// src/core/d2client/panel_draw_aaec0.cpp -- D2Client.dll 1.13c, base 0x6FAB0000.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

// 1.13c D2Client+0xAAEC0 (1707 B, caller 0xC39E0): bottom-panel / mini-panel button-bar draw.
// Every callee goes through the host seam in retail call order. Imports are rva = 0x100000|ordinal; the ordinal
// numbers are UNVERIFIED (read from the decompile, not matched against an import table). 10041 = draw cel
// (s[0]=frame, s[1]=x, s[2]=y, s[3]=-1, s[4]=5, s[5]=0 -> s[3] holds 4 words, rest in ecx/edx); the cel struct
// pointer is passed as its frame word. The 3rd draw's frame (stack0xfffffe84) is assumed 2 (unverified).
struct D2cPanelG {
    int32_t unitMode;          // *(bbfc+0x10) when bbfc != 0; bbfc==0 -> pass -1 here and has_unit=0
    int32_t has_unit;          // DAT_6fbcbbfc != 0
    int32_t d5d0, aadd0, aad84, aad90, W /*8bc48*/, H /*8bc4c*/, h9e14, dx /*cb9a0*/, dy /*cb9a4*/;
    int32_t add6, c973b, g9854, c0a0, dbc, dbd, dda, dde; // 6fbacdd6, 6fbc973b, 6fbc9854, 6fbcc0a0, acdbc, acdbd, acdda, acdde
    int32_t cc418, cc414, g9c4;                           // written
    const uint8_t* tab5;  // 6fbacd8c: dda entries of {i32 id; u8 enabled}
    const int32_t* tab6;  // 6fb8e6d8: dda entries of 6 i32 (x,y,?,y2,..)
};
static const uint32_t O = 0x100000;
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_aaec0(D2cPanelG* g, D2cHost h) {
    auto call = [&](uint32_t rva, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0) {
        D2cCall k = {}; k.rva = rva; k.eax = a; k.ecx = b; k.edx = c; k.s[0] = d; k.s[1] = e; return h(&k);
    };
    auto cel = [&](int32_t f, int32_t x, int32_t y) { call(O | 10041, f, x, y, (uint32_t)-1, 5); };
    call(0xAA3E0);
    if (!g->has_unit || g->unitMode == 0x11) {
        call(O | 10227);
        if (g->d5d0 != 0x26) { call(O | 10265); call(O | 10024); return -1; }   // _exit(-1)
        int32_t prev = g->aadd0; g->aadd0 = 0;
        if (prev) { uint32_t s = call(O | 10048); call(0x3CC60, s); call(0xAA380); call(0xBE2E0); }
        int32_t a, b;
        if (!g->aad84 && !g->aad90) { g->cc418 = 0; g->cc414 = 0; call(O | 10006); b = 0; a = g->h9e14; }
        else {
            g->cc418 = -((g->W + ((g->W >> 31) & 3)) >> 2); g->cc414 = 1; call(O | 10006);
            b = (g->h9e14 + ((g->h9e14 >> 31) & 3)) >> 2; a = g->h9e14 - b; b = -b;
        }
        call(0x8A4D0, a, 0, b); call(0xAA380);
    }
    call(O | 10184);
    cel(0, g->dx, g->dy - 0xE0 + g->H);
    cel(1, g->dx + 0x100, g->dy - 0xE0 + g->H);
    cel(2, g->dx, g->dy - 0x30 + g->H);
    cel(3, g->dx + 0x100, g->dy - 0x30 + g->H);
    static const int32_t o4[4] = {3, 0x51, 0x9f, 0xed}, o5[5] = {5, 0x43, 0x81, 0xbf, 0xfd};
    int32_t one, n; const int32_t* off;
    if (!call(O | 10227) || !g->g9854) { one = 0; n = 4; off = o4; } else { one = 1; n = 5; off = o5; }
    for (int32_t i = 0; i < n; i++) {
        bool draw = true; int32_t f = i * 2 + 1;
        if (i == g->add6) f = i * 2;
        else if (i >= 1 && i <= 4) {
            static const uint32_t k[5] = {0, 7, 0xF, 0x17, 0x1A};
            uint32_t r = call(O | 10174, g->c973b, k[i], 0);
            if (i == 4 && !(call(O | 10227) && g->g9854)) draw = false; else if (!r) draw = false;
        }
        if (draw) cel(f, off[i] + g->dx, (one - g->dy) + 0x21);
    }
    if (!g->g9c4) { g->g9c4 = (int32_t)call(0x2B420, 0); }   // wsprintfA("%s\\ui\\panel\\buysellbtn") result unused by host
    cel((g->dbc != 0) + 10, g->dx + 0x111, 0x1A1 - g->dy);
    int32_t sel = (int32_t)call(O | 10826, call(O | 10331));
    for (int32_t i = 0; i < g->dda; i++) {
        const uint8_t* e = g->tab5 + i * 5; const int32_t* t = g->tab6 + i * 6;
        int32_t id = (int32_t)(e[0] | e[1] << 8 | e[2] << 16 | (uint32_t)e[3] << 24);
        uint32_t u;
        if (!e[4]) u = 5;
        else { cel(id == sel ? (g->dbd == i) : (g->dbd == i) + 3, t[0] + g->dx, t[1] - g->dy); u = 0; }
        if (!g->dbc && g->dbd != -1 && i == g->dbd) u = 3;
        if (id == sel) { u = 3; cel(0, t[0] + g->dx, t[1] - g->dy); }
        if (!call(O | 10014, id)) call(O | 10003);
        else if (id == 8) {
            if (!g->c0a0) { uint32_t m = call(O | 10265, 0x60); call(O | 10024, m); return -1; }
            call(O | 10003);
        }
        call(O | 10150, t[3] - g->dy, u, 0);
    }
    call(O | 10003); call(O | 10028);
    call(O | 10150, 0x30 - g->dy, 0, 0);
    return 0;
}
