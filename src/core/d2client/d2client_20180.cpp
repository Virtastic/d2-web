// 1.13c D2Client+0x20180 -- object (type 3) paired effect spawn: fastcall ECX = object, one stack word p1 (ret 4).
// Gates: p1, object, type 3; owner = 0xA60D0(EAX = object); the object's id indexes the 0xB64/0xB6C table and the
// row's byte +5 must miss gdwBitMasks byte +0x10; 0x1FD00(EAX = owner, 0x80) must be 0. The anchor is 0xA5F70's
// unit (ESI = owner) position, else the object's path getters. Two 0xB0450 spawns (ESI = owner) one tile either
// side of the anchor, perpendicular to the owner's direction (none for ids 0x3F..0x42), with the type-1 owner's
// byte +0x16 bit 7 held set across them.
#include "cl_env.h"
extern "C" void d2c_20180(uint32_t o, uint32_t p1, const cl::Env& e) {
    if (!p1 || !o || e.R(o) != 3) return;
    int32_t id = (int32_t)e.R(o + 4);
    uint32_t s = e.H(0xa60d0, 0, 0, 0, 0, 0, o);
    if (!s) return;
    uint32_t row = e.Row(id);
    if (!row) return;
    if (e.R8(row + 5) & e.R8(e.bm + 0x10)) return;
    if (e.H(0x1fd00, 0x80, 0, 0, 0, 0, s)) return;
    uint32_t path = e.R(o + 0x2c);
    uint32_t v = e.H(0xa5f70, 0, 0, 0, 0, 0, 0, 0, 0, 0, s);
    uint32_t x0, y0;
    if (!v) { x0 = e.H(0xc31c, path); y0 = e.H(0xc340, path); }
    else { x0 = e.PD(v, 0xc3be, 0xc); y0 = e.PD(v, 0xc2ec, 0x10); }
    uint32_t seed[2] = {0, 0};
    uint32_t sp = (uint32_t)(uintptr_t)&seed[0];
    e.H(0xc634, 0, 0, 0, 0, 0, 0, sp);
    e.H(0xc3b2, 0, 0, 0, 0, 0, 0, sp, 0x53454953);
    int32_t ddx = (int32_t)(e.PD(s, 0xc3be, 0xc) - x0);
    int32_t ddy = (int32_t)(e.PD(s, 0xc2ec, 0x10) - y0);
    int32_t dx = ddx < 0 ? -1 : ddx > 0, dy = ddy < 0 ? -1 : ddy > 0;
    if (id >= 0x3f && id <= 0x42) dx = dy = 0;
    uint32_t f = (e.R(s) == 1) ? e.R(s + 0x14) : 0;
    if (f) e.W(f + 0x14, e.R(f + 0x14) | 0x00800000u);
    uint32_t a = e.H(0xc3fa, o), r = e.H(0xc388, o);
    e.H(0xb0450, (uint32_t)id, x0 - (uint32_t)dy, (uint32_t)dx + y0, r, a, 0, 0, 0, 0, s);
    a = e.H(0xc3fa, o); r = e.H(0xc388, o);
    e.H(0xb0450, (uint32_t)id, (uint32_t)dy + x0, y0 - (uint32_t)dx, r, a, 0, 0, 0, 0, s);
    if (e.R(s) == 1 && (f = e.R(s + 0x14))) e.W(f + 0x14, e.R(f + 0x14) & ~0x00800000u);
}
