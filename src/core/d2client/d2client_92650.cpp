// 1.13c D2Client+0x92650 item-name hover text (no arguments, void; caller 0x93450). Globals: hovered unit 0x11BC38, ui mode
// 0x11BC34, text anchors 0xE0EA8/0xE0EAC (x, y above) and 0xE0EB0/0xE0EB4 (x, y below). Three zeroed UTF-16 frame buffers:
// A (1024), B (128), O (1024). sep = 0xD1EC(ECX 0xF9E). No unit, 0xC220(unit) != 0x12 or 0xC1BA(unit+4 id) == 0: Fog fatal
// (0xBF92 code 0xCF1/0xCF5, 0xBF80(RVA 0xD359F, msg), exit(-1) 0x3664). 0x8D880 (ESI = A; unit) fills A; ui mode 0: A +=
// 0xD1EC(0x89B), sep, 0xD1EC(0x89E), sep. 0x914F0(unit, B, 0x80) fills B; A += B. Mode 1..9: 0x91360 (ESI = O; A), else O = A.
// 0xD3B4 (ECX O, EDX &W; &H) measures O; [0xE0EAC] - H > 0: 0xD384 (ECX O, EDX [0xE0EA8]; [0xE0EAC], 0, 0) else (EDX [0xE0EB0];
// [0xE0EB4] + H, 0, 0).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c92650G { int32_t mode; uint32_t unit; int32_t xa, ya, xb, yb; };
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
static void wcat(char16_t* d, const char16_t* s) { while (*d) ++d; while ((*d++ = *s++)) {} }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_92650(const D2cSvc& v, const D2c92650G* g) {
    struct { int32_t H, W; char16_t B[128], A[1024], O[1024]; } f = {};
    const char16_t* sep = (const char16_t*)(uintptr_t)d2c_call(v, 0xd1ec, 0, 0xf9e, 0, 0, 0);
    uint32_t code = 0xcf1;
    if (g->unit && d2c_call(v, 0xc220, 0, 0, 0, 0, 0, g->unit) == 0x12) {
        uint32_t id; memcpy(&id, (const void*)(uintptr_t)(g->unit + 4), 4);
        if (d2c_call(v, 0xc1ba, 0, 0, 0, 0, 0, g->unit ? id : 0xffffffffu)) {
            d2c_call(v, 0x8d880, 0, 0, 0, 0, P(f.A), g->unit);
            if (g->mode == 0) {
                wcat(f.A, (const char16_t*)(uintptr_t)d2c_call(v, 0xd1ec, 0, 0x89b, 0, 0, 0)); wcat(f.A, sep);
                wcat(f.A, (const char16_t*)(uintptr_t)d2c_call(v, 0xd1ec, 0, 0x89e, 0, 0, 0)); wcat(f.A, sep);
            }
            d2c_call(v, 0x914f0, 0, 0, 0, 0, 0, g->unit, P(f.B), 0x80);
            wcat(f.A, f.B);
            if (g->mode >= 1 && g->mode <= 9) d2c_call(v, 0x91360, 0, 0, 0, 0, P(f.O), P(f.A));
            else { f.O[0] = 0; wcat(f.O, f.A); }
            d2c_call(v, 0xd3b4, 0, P(f.O), P(&f.W), 0, 0, P(&f.H));
            if ((int32_t)((uint32_t)g->ya - (uint32_t)f.H) > 0) d2c_call(v, 0xd384, 0, P(f.O), (uint32_t)g->xa, 0, 0, (uint32_t)g->ya, 0, 0);
            else d2c_call(v, 0xd384, 0, P(f.O), (uint32_t)g->xb, 0, 0, (uint32_t)g->yb + (uint32_t)f.H, 0, 0);
            return;
        }
        code = 0xcf5;
    }
    const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, code);
    d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xd359f, m);
    d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    std::_Exit(-1);
}
