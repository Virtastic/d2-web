// D2Client+0xB1FC0 -- one client step of a following missile: turn or hold by the owner unit's id class and the distance.
#include "d2client_b1fc0.h"

// sub-block of the missile's path object: m+0x54, then +0x30 (null-checked on every use, as retail)
static U sub(const D2ClientB1fc0Env& e, U m) { U p = e.ldp(m, 0x54); return p ? e.ldp(p, 0x30) : 0; }
static void setVel(const D2ClientB1fc0Env& e, U m, uint32_t v) {
    if (U s = sub(e, m)) { e.st32(s, 0x10, 0); e.st32(s, 0x14, 0); e.st32(s, 0x18, v); }
}
static void setAcc(const D2ClientB1fc0Env& e, U m, uint32_t v) {
    if (U s = sub(e, m)) { e.st32(s, 0x28, 0); e.st32(s, 0x2c, 0); e.st32(s, 0x30, v); }
}
static void flag(const D2ClientB1fc0Env& e, U m, bool set) {
    if (U s = sub(e, m)) { uint32_t f = e.ld32(s, 0); e.st32(s, 0, set ? f | 0x20 : f & ~0x20u); }
}

// 1.13c D2Client+0xB1FC0
void d2c_b1fc0(const D2ClientB1fc0Env& e, U m) {
    const uint32_t mode = e.f10818(m);              // EAX at the compare below
    const uint32_t ow = e.ld32(m, 0x2c);
    if (!ow) { e.tail(m); return; }
    const uint32_t id = e.f11018(m);
    U o = e.ldp(e.tbl, (id & 0x7f) * 4u);   // retail chain heads are 32-bit words
    while (o && e.ld32(o, 0xc) != id) o = e.ldp(o, 0xe4);
    if (!o) { e.tail(m); return; }
    if (e.ld32(o, 0) != 3) e.die();
    const int32_t st = (int32_t)e.f11018(o);
    const int32_t dist = (int32_t)e.f10522(m, o);
    const uint64_t s = (uint64_t)e.ld32(m, 0x20) * 0x6ac690c5u + e.ld32(m, 0x24);
    e.st32(m, 0x20, (uint32_t)s); e.st32(m, 0x24, (uint32_t)(s >> 32));
    uint32_t lim = (uint32_t)s % 5; if (lim <= 1) lim = 1;
    if (mode == (uint32_t)st || st <= 1) {
        if (dist < (int32_t)lim && mode == 0) {
            e.f10019(m, 1); e.f77bf0(m);
            if (U x = sub(e, m)) { e.st32(x, 0x10, 0); e.st32(x, 0x14, 0); e.st32(x, 0x18, 0x800); }
        } else if (mode == 1) {
            const uint32_t xo = e.f1630(o), xm = e.f1630(m);
            const uint32_t yo = e.f1660(o), ym = e.f1660(m);
            const int32_t dx = (int32_t)(xm - xo), dy = (int32_t)(ym - yo);
            if (!dx && !dy) { e.tail(m); return; }
            const uint32_t y = e.f1660(m) + (uint32_t)dx;
            const uint32_t x = e.f1630(m) - (uint32_t)dy;
            e.f10286(ow, x, y); e.f10488(ow, 0xa00, 0x179e); e.f10334(ow, m, 0);
        } else if (mode == 3) {
            const uint32_t y = e.f1660(o), x = e.f1630(o);
            e.f10286(ow, x, y); e.f10488(ow, 0x200, 0x17a7); e.f10334(ow, m, 0);
        }
    } else {
        e.f10019(m, (uint32_t)st);
        if (st == 2) {
            e.f10488(ow, 0, 0x1774); e.f77bf0(m);
            setVel(e, m, 0x3000);
            const uint32_t r = e.f1330(m + 0x20, 0x28) + 0xaa; setAcc(e, m, r);
            flag(e, m, true);
            const uint32_t y = e.f1660(o), x = e.f1630(o);
            e.f10286(ow, x, y); e.f10488(ow, 0x300, 0x177c); e.f10334(ow, m, 0);
        } else if (st == 3) {
            e.f77bf0(m);
            setVel(e, m, 0xffffd000u);
            const uint32_t r = e.f1330(m + 0x20, 0x28); setAcc(e, m, r);
            flag(e, m, false);
            e.f10334(ow, m, 0);
        } else if (st == 4) {
            e.f77bf0(m);
            setVel(e, m, 0xa000);
            setAcc(e, m, 0x190);
            flag(e, m, true);
            e.f10334(ow, m, 0);
        }
    }
    e.tail(m);
}
