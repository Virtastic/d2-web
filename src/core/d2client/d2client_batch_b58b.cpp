// D2Client batch b58b: callees go through D2cHost (ordinals as 0x80000000|n); globals passed by pointer (RVA noted).
#include <initializer_list>
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
u32 ord(D2cHost h, u32 n, u32 a = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) {
    D2cCall c = {}; c.rva = 0x80000000u | n; c.eax = a; c.ecx = a; c.s[0] = a; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    return h(&c);
}
}
struct D2cB58G { i32 *state /*B925F0*/, *mouseY /*1CB828*/, *mouseX /*1CB824*/, *gW /*1B8BC48*/, *gH /*1B8BC4C*/, *offX /*1CB9A0*/, *offY /*1CB9A4*/; };
extern "C" {
// 1.13c D2Client+0x73B60  (ECX = unit)
EMSCRIPTEN_KEEPALIVE void d2c_73b60(D2cHost h, const uint8_t* unit, u32 unitHandle) {
    if (!unit) return;
    i32 lvl = (i32)ord(h, 10534, unitHandle);
    if (lvl > 0x4d) { lvl = 0x4d; ord(h, 10558, unitHandle, 0x4d); ord(h, 10970, unitHandle, 0x4d); }
    u32 f; memcpy(&f, unit + 0x2c, 4);
    ord(h, 10983, f); ord(h, 10614); ord(h, 10647, f, 10); ord(h, 10168, f, (u32)lvl); ord(h, 10334, f, unitHandle, 0);
}

// 1.13c D2Client+0x77DE0
EMSCRIPTEN_KEEPALIVE void d2c_77de0(D2cHost h, D2cB58G* g) {
    i32 base = *g->gW - *g->offX, lo, hi;
    if (*g->state == 1) { lo = base - 0x95; hi = base - 0x73; }
    else if (*g->state == 2) { lo = base - 0xdc; hi = base - 0xba; }
    else if (*g->state == 3) { lo = base - 0x131; hi = base - 0x10f; }
    else return;
    if (*g->mouseY < lo || *g->mouseY > hi) return;
    i32 y = *g->offY - 0x3f + *g->gH;
    if (y - 0x22 <= *g->mouseX && *g->mouseX <= y) { ord(h, 10003, (u32)(*g->offY - 0x62 + *g->gH), 0, 1); ord(h, 10085); }
}
}
