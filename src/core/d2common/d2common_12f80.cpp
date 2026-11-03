// 1.13c D2Common+0x12F80: build DrlgAnim records. EAX = array of 0x30-byte cell entries (the first field used is at +0x18);
// stack: drlg, default (0 means 0x50), count, tile-grid table. Retail's callees are seams with their real register/stack split:
//   typeFlags  import 0x92FA (stdcall 1 word)         pick  import 0x9306 (stdcall 6 words)
//   alloc      Fog alloc 0x9222 (EDX = size; stack file, line, 0)
//   rank       0x930c (stdcall 1 word); the first pass skips tiles for which it is non-zero, the second pass wants rank == s
//   mk0 0x68DF0 (EAX drlg, EDX x0, ECX y0; stack v, tile)   mkD 0x68CD0 (ECX drlg, EDX x0; stack 0, y0, tile)
//   mkN 0x68F00 (ECX drlg, EDX 0; stack x0, y0, v, tile, type)   fatal 0x921C/0x9216/0x1B0D path (noreturn in retail)
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& W(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(P(a) + o); }
}

extern "C" {
struct D2Common12F80Env {
    uint32_t (*typeFlags)(uint32_t id);
    uint32_t (*pick)(uint32_t pool, uint32_t type, uint32_t b, uint32_t c, uint32_t* buf, uint32_t size);
    uint32_t (*alloc)(const char* file, int32_t line, int32_t z, uint32_t size);
    uint32_t (*rank)(uint32_t tile);
    void (*mk0)(uint32_t drlg, uint32_t x0, uint32_t y0, uint32_t v, uint32_t tile);
    uint32_t (*mkD)(uint32_t drlg, uint32_t x0, uint32_t y0, uint32_t tile);
    uint32_t (*mkN)(uint32_t drlg, uint32_t x0, uint32_t y0, uint32_t v, uint32_t tile, uint32_t ty);
    void (*fatal)(uint32_t code);
};

// 1.13c D2Common+0x12F80
EMSCRIPTEN_KEEPALIVE void d2_port_d2common_12f80(uint32_t arr, uint32_t drlg, int32_t dflt, int32_t count, uint32_t tbl, const D2Common12F80Env* E) {
    if (!arr) return;
    if (!dflt) dflt = 0x50;
    const char* F = "..\\Source\\D2Common\\DRLG\\DrlgAnim.cpp";
    uint32_t t[40];
    uint32_t e = arr + 0x18;
    for (int32_t i = 0; i < count; ++i, e += 0x30) {
        if (!W(e, 0) || !(E->typeFlags(W(e, 0)) & 0x100)) continue;
        const uint32_t g = tbl - 0x14 + ((W(e, -4) >> 14) & 7) * 0x14;
        const uint32_t v = W(W(g, 0) + (W(W(g, 4) + W(e, -12) * 4, 0) + W(e, -16)) * 4, 0);
        const uint32_t bt = v ? (v >> 20) & 0x3f : 0, ix = v ? v >> 8 : 0;
        const uint32_t n = E->pick(drlg + 0x68, W(e, 4), bt, ix & 0xff, t, 0x28);
        const uint32_t r = E->alloc(F, 0x9b, 0, 0x14);
        memset(P(r), 0, 20);
        W(r, 4) = n;
        W(r, 0) = E->alloc(F, 0xa1, 0, n * 4);
        memset(P(W(r, 0)), 0, n * 4);
        W(r, 12) = dflt;
        const uint32_t lvl = W(drlg, 0x54);
        W(r, 16) = W(lvl, 4);
        W(lvl, 4) = r;
        uint32_t k = 0;
        while (k < n && E->rank(t[k])) ++k;
        if (k >= n) { E->fatal(0xb6); return; }
        W(e, 0) = t[k];
        W(W(r, 0), 0) = e - 24;
        const uint32_t x0 = W(e, -16) + W(drlg, 0x34), y0 = W(e, -12) + W(drlg, 0x38);
        for (uint32_t s = 1; s < n; ++s) {
            uint32_t j = 0;
            while (j < n && E->rank(t[j]) != s) ++j;
            if (j >= n) { E->fatal(0xcb); return; }
            uint32_t o;
            const uint32_t ty = W(e, 4);
            if (!ty) {
                o = W(lvl, 0x1c) + W(lvl, 0xc) * 0x30;
                W(o, 32) = 0;
                W(lvl, 0xc)++;
                E->mk0(drlg, x0, y0, v, t[j]);
            } else if (ty == 0xd) o = E->mkD(drlg, x0, y0, t[j]);
            else o = E->mkN(drlg, x0, y0, v, t[j], ty);
            W(W(r, 0), s * 4) = o;
            W(o, 20) |= 8;
        }
    }
}
}
