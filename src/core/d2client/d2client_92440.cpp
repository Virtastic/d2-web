// 1.13c D2Client+0x92440 hover text for a unit of the second kind (no arguments, void; caller 0x93450). Globals: hovered unit
// 0x11BC38, ui mode 0x11BC34, text anchors 0xE0EA8/0xE0EAC (above) and 0xE0EB0/0xE0EB4 (below). Frame: A (1024 UTF-16, zeroed),
// B (128), O (1024), the word X (0) and the measured W/H. sep = 0xD1EC(ECX 0xF9E). 0xC856(unit) == 0: return. t = 0xC1BA(unit ?
// unit+4 : -1), zero: Fog fatal 0xE61. p = 0xC94C(t+0xC8; &X), zero: fatal 0xE64. A = str(0x150B), then A = str(word p+0xF4) (the
// second strcpy overwrites the first; ECX's high half is what the strcpy import left, 0 here), A += sep, str(0x150B), sep.
// 0x914F0(unit, B, 0x80), A += B. Mode 1..9: 0x91360 (ESI = O; A) else O = A. 0xD3B4 (ECX O, EDX &W; &H); [0xE0EAC] - H > 0:
// 0xD384 (ECX O, EDX [0xE0EA8]; [0xE0EAC], 0, 0) else (EDX [0xE0EB0]; [0xE0EB4] + H, 0, 0). Fatal tail as 0x92650.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c92440G { int32_t mode; uint32_t unit; int32_t xa, ya, xb, yb; };
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
static const char16_t* S(uint32_t p) { return (const char16_t*)(uintptr_t)p; }
static void wcpy(char16_t* d, const char16_t* s) { while ((*d++ = *s++)) {} }
static void wcat(char16_t* d, const char16_t* s) { while (*d) ++d; wcpy(d, s); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_92440(const D2cSvc& v, const D2c92440G* g) {
    struct { uint32_t X, H, W; char16_t B[128], A[1024], O[1024]; } f = {};
    const char16_t* sep = S(d2c_call(v, 0xd1ec, 0, 0xf9e, 0, 0, 0));
    if (!d2c_call(v, 0xc856, 0, 0, 0, 0, 0, g->unit)) return;
    uint32_t id = 0xffffffffu;
    if (g->unit) memcpy(&id, (const void*)(uintptr_t)(g->unit + 4), 4);
    const uint32_t t = d2c_call(v, 0xc1ba, 0, 0, 0, 0, 0, id);
    uint32_t code = 0xe61;
    if (t) {
        uint32_t k; memcpy(&k, (const void*)(uintptr_t)(t + 0xc8), 4);
        const uint32_t p = d2c_call(v, 0xc94c, 0, 0, 0, 0, 0, k, P(&f.X));
        if (p) {
            uint16_t nm; memcpy(&nm, (const void*)(uintptr_t)(p + 0xf4), 2);
            wcpy(f.A, S(d2c_call(v, 0xd1ec, 0, 0x150b, 0, 0, 0)));
            wcpy(f.A, S(d2c_call(v, 0xd1ec, 0, nm, 0, 0, 0)));
            wcat(f.A, sep);
            wcat(f.A, S(d2c_call(v, 0xd1ec, 0, 0x150b, 0, 0, 0)));
            wcat(f.A, sep);
            d2c_call(v, 0x914f0, 0, 0, 0, 0, 0, g->unit, P(f.B), 0x80);
            wcat(f.A, f.B);
            if (g->mode >= 1 && g->mode <= 9) d2c_call(v, 0x91360, 0, 0, 0, 0, P(f.O), P(f.A));
            else wcpy(f.O, f.A);
            d2c_call(v, 0xd3b4, 0, P(f.O), P(&f.W), 0, 0, P(&f.H));
            if ((int32_t)((uint32_t)g->ya - f.H) > 0) d2c_call(v, 0xd384, 0, P(f.O), (uint32_t)g->xa, 0, 0, (uint32_t)g->ya, 0, 0);
            else d2c_call(v, 0xd384, 0, P(f.O), (uint32_t)g->xb, 0, 0, (uint32_t)g->yb + f.H, 0, 0);
            return;
        }
        code = 0xe64;
    }
    const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, code);
    d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xd359f, m);
    d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    std::_Exit(-1);
}
