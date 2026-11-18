// 1.13c D2Client+0x87940 wardenRequestModule (EBX = 16-byte module key, one stack word a0, ret 4). Global: ctx 0x11BF54 (retail
// pointers are 32-bit). 0xBEDE (stdcall: key, RVA 0xD26F8, 0x10) returning 0: 0x87560(ECX 0x12B2; RVA 0xD1440, RVA 0xD2708).
// Else the key is written as 32 lowercase hex digits into a frame local (CRT sprintf "%02x" per byte, RVA 0xD4930), Storm
// 0xBE96(&hex[32], RVA 0xD4928, 5) copies the extension after it; no ctx: 0. ctx+0xCC (ECX hex, EDX a local; &Lc, &L8)
// returning 0: 0; else r = 0x87560(ECX L8; Lc, a0), ctx+0xC8 (ECX Lc, EDX L8), return r. Indirect targets are keyed by pointer.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_87940(const D2cSvc& v, uint32_t key, uint32_t a0, const uint32_t* gctx) {
    if (!d2c_call(v, 0xbede, 0, 0, 0, 0, 0, key, 0xd26f8, 0x10)) return d2c_call(v, 0x87560, 0, 0x12b2, 0, 0, 0, 0xd1440, 0xd2708);
    struct { uint32_t L8, Lc, loc[2]; char hex[0x30]; } f = {};
    static const char hx[] = "0123456789abcdef";
    const uint8_t* k = (const uint8_t*)(uintptr_t)key;
    for (int i = 0; i < 16; ++i) { f.hex[2 * i] = hx[k[i] >> 4]; f.hex[2 * i + 1] = hx[k[i] & 15]; }
    d2c_call(v, 0xbe96, 0, 0, 0, 0, 0, P(f.hex + 32), 0xd4928, 5);
    const uint32_t c = *gctx;
    if (!c) return 0;
    if (!d2c_call(v, rd32(c + 0xcc), 0, P(f.hex), P(f.loc), 0, 0, P(&f.Lc), P(&f.L8))) return 0;
    const uint32_t r = d2c_call(v, 0x87560, 0, f.L8, 0, 0, 0, f.Lc, a0);
    d2c_call(v, rd32(c + 0xc8), 0, f.Lc, f.L8, 0, 0);
    return r;
}
