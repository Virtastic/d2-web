// 1.13c D2Client+0x7EFA0 panel-state open (EAX record, stack unit-or-0; ret 4). Globals (addresses in G, retail pointers 32-bit):
// panel-active table 0xFAD80 (index 0x10 = 0xFADC0 the side-panel toggle, 1 = 0xFAD84, 4 = 0xFAD90), panel records 0xDD538 (+0x40
// kind) with their count 0xDD5D0 (must be 0x26, else the fatal 0xBF92/0xBF80/0x3664 path), block flag 0x113A68, current record
// 0x113AE8 (:= EAX), 0x113AA0, 0x113B14, viewed unit 0x11BBFC, 0x11971D (EDI of 0x47B20) / 0x119721 flag, screen width 0xDBC48,
// view width / height 0xF9E14 / 0xF9E18, 0x11C3C4 (ESI of 0x8A4D0), offsets 0x11C414 / 0x11C418, 0xF46C4 / 0xF46C8, 0x11C3DC /
// 0x11C3E0, 0x11C3F8 / 0x11C3FC, 0x11C2F4 / 0x11C2F8. 0xBF8C first. Toggle clear and no block: each active panel of kind 1 is
// closed by 0xC2790 (ECX i, EDX 1; 0), kind 4 runs 0x47B20 when 0x119721 is set, kind 0 is skipped, kinds 2, 3 and > 4 jump to the
// tail. Then no unit, or a type-0 unit 0xA5440 rejects, flips the toggle (0xA5440 leaves EDX alone). The toggle's change runs
// 0x3CCC0 (opened) or 0x3CC60 + 0xBE2E0 (closed), each (EAX 2; 0xD0B4(0xD0810)). The shift (W/4 by the toggle and the two panel
// flags) goes to 0x11C418 / 0x11C414 and 0xD1B0 (ECX), the view to 0x8A4D0 (EAX h, ECX w', EDX 0, ESI; dx). Tail: 0x10 into
// 0xF46C4/C8, six words cleared; a record of kind 'bkd ' (+0x80) sends packet 0x3E + the unit id (+0xC, or -1) through 0x143E0
// (EBX 5; &pkt; its EAX is returned) with 0x113B14 = 1, 0x113AA0 = 0; any other kind 0x113B14 = 0, 0x113AA0 = 1, EAX 0x10.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c7efa0G { uint32_t act, recs, h, unit, q, scrW, view, g3c4, g2f4, f46c4, str; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
static int32_t q4(int32_t x) { return (x + (int32_t)((uint32_t)(x >> 31) & 3u)) >> 2; }
// 1.13c D2Client+0xA5440: the unit cannot open a panel (+0xC4 bit 0x10000, a type-0 unit in mode 0 or 0x11, type 1 in 0 or 0xC)
static uint32_t d2c_a5440_inline(uint32_t u) {
    if (rd32(u + 0xc4) & 0x10000) return 1;
    const uint32_t m = rd32(u + 0x10), t = rd32(u);
    if (t == 0) return (m == 0 || m == 0x11) ? 1 : 0;
    if (t == 1) return (m == 0 || m == 0xc) ? 1 : 0;
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_7efa0(const D2cSvc& v, const D2c7efa0G* g, uint32_t rec, uint32_t arg) {
    const uint32_t tog = g->act + 0x40, old = rd32(tog);
    wr32(g->h + 0x80, rec);
    d2c_call(v, 0xbf8c, 0, 0, 0, 0, 0);
    const uint32_t cnt = g->recs + 0x98;
    if (rd32(cnt) != 0x26) {
        const uint32_t s = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x139b);
        d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB20D359Fu, s);
        d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    uint32_t edx = rd32(tog);
    bool tail = false;
    if (edx == 0) {
        if (rd32(g->h) != 0) tail = true;
        for (uint32_t i = 0; !tail && (int32_t)i < (int32_t)rd32(cnt); ++i) {
            if (rd32(g->act + 4 * i) != 1) continue;
            const uint32_t k = rd32(rd32(g->recs + 4 * i) + 0x40);
            if (k == 1) { d2c_call(v, 0xc2790, 0, i, 1, 0, 0, 0); edx = rd32(tog); }
            else if (k == 4) { if (rd32(g->q + 4)) { d2c_call(v, 0x47b20, 0, 0, 0, 0, 0); edx = rd32(tog); } }
            else if (k != 0) tail = true;
        }
    }
    if (!tail) {
        const uint32_t u = rd32(g->unit);
        if (!u || (rd32(u) == 0 && !d2c_a5440_inline(u))) edx = edx == 0;
        wr32(tog, edx);
        if (old == 0) {
            if (edx) d2c_call(v, 0x3ccc0, 2, 0, 0, 0, 0, d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, g->str));
        } else if (edx == 0) {
            d2c_call(v, 0x3cc60, 2, 0, 0, 0, 0, d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0, g->str));
            d2c_call(v, 0xbe2e0, 0, 0, 0, 0, 0);
        }
        int32_t c, push;
        const uint32_t o414 = g->g3c4 + 0x50, o418 = g->g3c4 + 0x54;
        if (rd32(g->act + 4) == 0 && rd32(g->act + 0x10) == 0) {
            if (edx) {
                const int32_t s = q4((int32_t)rd32(g->scrW));
                wr32(o418, (uint32_t)s); wr32(o414, 2); d2c_call(v, 0xd1b0, 0, (uint32_t)s, 0, 0, 0);
                c = (int32_t)rd32(g->view); push = q4(c); c += push;
            } else {
                wr32(o418, 0); wr32(o414, 0); d2c_call(v, 0xd1b0, 0, 0, 0, 0, 0);
                c = (int32_t)rd32(g->view); push = 0;
            }
        } else if (edx) {
            wr32(o418, 0); wr32(o414, 3); d2c_call(v, 0xd1b0, 0, 0, 0, 0, 0);
            c = (int32_t)rd32(g->view); push = 0;
        } else {
            const int32_t s = -q4((int32_t)rd32(g->scrW));
            wr32(o418, (uint32_t)s); wr32(o414, 1); d2c_call(v, 0xd1b0, 0, (uint32_t)s, 0, 0, 0);
            c = (int32_t)rd32(g->view); const int32_t a4 = q4(c); c -= a4; push = -a4;
        }
        d2c_call(v, 0x8a4d0, rd32(g->view + 4), (uint32_t)c, 0, 0, rd32(g->g3c4), (uint32_t)push);
    }
    wr32(g->f46c4 + 4, 0x10); wr32(g->f46c4, 0x10);
    wr32(g->g3c4 + 0x18, 0); wr32(g->g3c4 + 0x1c, 0); wr32(g->g3c4 + 0x34, 0); wr32(g->g3c4 + 0x38, 0);
    wr32(g->g2f4 + 4, 0); wr32(g->g2f4, 0);
    if (rd32(rd32(g->h + 0x80) + 0x80) == 0x20646b62) {
        wr32(g->h + 0xac, 1);
        const uint32_t id = arg ? rd32(arg + 0xc) : 0xffffffffu;
        uint8_t pkt[8] = {}; pkt[0] = 0x3e; memcpy(pkt + 1, &id, 4);
        const uint32_t ax = d2c_call(v, 0x143e0, 0, 0, 0, 5, 0, (uint32_t)(uintptr_t)pkt);
        wr32(g->h + 0x38, 0);
        return ax;
    } else {
        wr32(g->h + 0xac, 0);
        wr32(g->h + 0x38, 1);
        return 0x10;   // EAX still holds the 0x10 stored above
    }
}
