// 1.13c D2Client+0x91360 item-name suffix (ESI = out, stdcall text, ret 4; callers 0x92440, 0x92650, 0x93450). Globals: ui mode
// 0x11BC34, hovered unit 0x11BC38, owner id 0xE0EA4. Frame: L (64 UTF-16, zeroed), T (128). sep = 0xD1EC(ECX 0xF9E); out = text.
// Mode 4 and 0xC1A8(unit, 0x400000, 0xCCC, inv.cpp) != 0: T = str(0x58DA), out += sep, out += 0xBF5F0(EDI = T; 1). Then
// 0x46230 (EAX = &frame word, ECX unit; [0xE0EA4], L, 0x40) non-zero: L[0] != 0 adds sep, out += L, done. Zero and mode != 4:
// T = str(0xD05), out += sep, out += 0xBF5F0(EDI = T; 1). The colour helper 0xBF5F0 takes EDI, carried as register word 5.
#include <cstdint>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c91360G { int32_t mode; uint32_t unit; uint32_t owner; };
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
static const char16_t* S(uint32_t p) { return (const char16_t*)(uintptr_t)p; }
static void wcpy(char16_t* d, const char16_t* s) { while ((*d++ = *s++)) {} }
static void wcat(char16_t* d, const char16_t* s) { while (*d) ++d; wcpy(d, s); }
static uint32_t colour(const D2cSvc& v, char16_t* t) {   // 0xBF5F0: EDI = t, stack word 1
    const uint32_t r[6] = { 0, 0, 0, 0, 0, P(t) }, s[6] = { 1, 0, 0, 0, 0, 0 };
    return v.call(v.ctx, 0xbf5f0, r, s);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_91360(const D2cSvc& v, const D2c91360G* g, char16_t* out /*esi*/, const char16_t* text) {
    struct { uint32_t w; char16_t L[64]; char16_t T[128]; } f = {};
    const char16_t* sep = S(d2c_call(v, 0xd1ec, 0, 0xf9e, 0, 0, 0));
    wcpy(out, text);
    if (g->mode == 4 && d2c_call(v, 0xc1a8, 0, 0, 0, 0, 0, g->unit, 0x400000, 0xccc, 0xB20D476Cu)) {
        wcpy(f.T, S(d2c_call(v, 0xd1ec, 0, 0x58da, 0, 0, 0)));
        wcat(out, sep);
        wcat(out, S(colour(v, f.T)));
    }
    if (d2c_call(v, 0x46230, P(&f.w), g->unit, 0, 0, 0, g->owner, P(f.L), 0x40)) {
        if (f.L[0]) wcat(out, sep);
        wcat(out, f.L);
        return;
    }
    if (g->mode != 4) {
        for (int i = 0; i < 64; ++i) f.T[i] = 0;
        wcpy(f.T, S(d2c_call(v, 0xd1ec, 0, 0xd05, 0, 0, 0)));
        wcat(out, sep);
        wcat(out, S(colour(v, f.T)));
    }
}
