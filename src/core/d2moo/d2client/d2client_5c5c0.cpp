// 1.13c D2Client+0x5C5C0 hireling / viewed-unit panel tick (no arguments, void). Globals (addresses in G, retail pointers 32-bit):
// m = view mode 0x11C284 (flags 0x11C294/0x11C298/0x11C2A0 at m+0x10/+0x14/+0x1C), v = viewed unit 0x11BBFC, n = hireling list
// 0x11C4D4 (node +4 type, +8 slot id, +0xC unit id, +0x20 flag, +0x30 next), o = open flag 0xFADCC (panel active [19]), t = panel table 0xDD538 (+0x4C
// kind; count at t+0x98 = 0xDD5D0), a = panel active 0xFAD80, k = 0x11971C (k+1 = 0x11971D the EDI of 0x47B20, k+5 = 0x119721 its
// gate), s = unit slots 0x10A808 (chain next +0xE4). Mode 2: clear the flags. own = a list node carries the viewed unit's id (-1 with
// no unit); hire = 0x9D9D0 (EAX = viewed unit). Not (hire && mode != 1) and not own: clear; open set: 0xBF8C, count must be 0x26
// (Fog fatal 0x139B), open = 0, 0x3CC60 (EAX 1; 0xD0B4(0xD0658)). Else, with a viewed unit, the first list node of type 7, its id and
// flag +0x20 clear names a slot id; -1 clears; the slot chain's unit with that id must be type 1 (fatal 0x5A) and clears when
// 0xA5440 (EAX unit) is non-zero; a chain dead end keeps the flags. No such node clears. Then open set: return. 0xBF8C, count 0x26;
// each active panel by kind: 1 0xC2790 (ECX i, EDX 1; 0), 4 0x47B20 when the gate is set, 2/3 return, 0 next. No viewed unit or
// (type 0 and 0xA5440 zero): open = 1; else open clear returns. 0x3CCC0 (EAX 1; 0xD0B4(0xD0658)).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c5c5c0G { uint32_t m, v, n, o, t, a, k, s; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

static void fatal(const D2cSvc& v, uint32_t code) {
    const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, code);
    d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xd359f, m);
    d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    std::_Exit(-1);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_5c5c0(const D2cSvc& v, const D2c5c5c0G* g) {
    const int32_t mode = (int32_t)rd32(g->m);
    auto clear = [&] { wr32(g->m + 0x1c, 0); wr32(g->m + 0x14, 0); wr32(g->m + 0x10, 0); };
    if (mode == 2) { clear(); return; }
    const uint32_t u = rd32(g->v), list = rd32(g->n);
    const uint32_t id = u ? rd32(u + 0xc) : 0xffffffffu;
    bool own = false;
    for (uint32_t n = list; n; n = rd32(n + 0x30)) if (rd32(n + 0xc) == id) { own = true; break; }
    const uint32_t hire = d2c_call(v, 0x9d9d0, u, 0, 0, 0, 0);
    if (!(hire && mode != 1) && !own) {
        const uint32_t was = rd32(g->o);
        clear();
        if (!was) return;
        d2c_call(v, 0xbf8c, 0, 0, 0, 0, 0);
        if (rd32(g->t + 0x98) != 0x26) fatal(v, 0x139b);
        wr32(g->o, 0);
        d2c_call(v, 0x3cc60, 1, 0, 0, 0, 0, d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, 0xd0658));
        return;
    }
    bool clr = true;
    if (u) {
        const uint32_t uid = rd32(u + 0xc);
        for (uint32_t n = list; n; n = rd32(n + 0x30)) {
            if (rd32(n + 4) != 7 || rd32(n + 0xc) != uid || rd32(n + 0x20)) continue;
            const uint32_t sid = rd32(n + 8);
            if (sid == 0xffffffffu) break;
            uint32_t x = rd32(g->s + (sid & 0x7f) * 4);
            for (; x && rd32(x + 0xc) != sid; x = rd32(x + 0xe4)) {}
            if (!x) clr = false;
            else {
                if (rd32(x) != 1) fatal(v, 0x5a);
                clr = d2c_call(v, 0xa5440, x, 0, 0, 0, 0) != 0;
            }
            break;
        }
    }
    if (clr) clear();
    if (rd32(g->o)) return;
    d2c_call(v, 0xbf8c, 0, 0, 0, 0, 0);
    if (rd32(g->t + 0x98) != 0x26) fatal(v, 0x139b);
    for (uint32_t i = 0; (int32_t)i < (int32_t)rd32(g->t + 0x98); ++i) {
        if (rd32(g->a + i * 4) != 1) continue;
        const uint32_t kind = rd32(rd32(g->t + i * 4) + 0x4c);
        if (kind == 1) d2c_call(v, 0xc2790, 0, i, 1, 0, 0, 0);
        else if (kind == 4) { if (rd32(g->k + 5)) d2c_call(v, 0x47b20, 0, 0, 0, 0, 0); }
        else if (kind != 0) return;
    }
    const uint32_t w = rd32(g->v);
    if (!w || (rd32(w) == 0 && !d2c_call(v, 0xa5440, w, 0, 0, 0, 0))) wr32(g->o, 1);
    else if (!rd32(g->o)) return;
    d2c_call(v, 0x3ccc0, 1, 0, 0, 0, 0, d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, 0xd0658));
}
