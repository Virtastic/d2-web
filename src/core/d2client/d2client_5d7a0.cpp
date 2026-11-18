// 1.13c D2Client+0x5D7A0 sound-play request (stdcall rq, ret 4; caller 0x88DD0). Globals: record table 0x11C24C (stride 0x8E),
// count 0x11C250 (retail pointers are 32-bit). rec = the record of rq+4 as it is on entry; rq+0x38 bit 0 clear: rq+4 = 0x5EE70(id).
// 0x5CC10(rq+4) returning a node whose entry (node+8) has +0x3D == 0, rq+0x34 - entry+0x34 < 2 (unsigned) and entry+0x3C >= rq+0x3C
// (bytes): 0. rec+0x68 = rec+0x64, rec+0x64 = rq+4; r2 = the record of rq+4. Unless r2+0x4F or r2+0x86 == 2: 0x5EBC0 (EAX = r2+0x4D
// == 0, ECX rq+4; 0) and then r2+0x86 must be 2. r2+0x81 set: 0. f = r2+0x50 ? 1 : (0xD324() == 1 ? 2 : 0) | (0xD324() == 2 ? 6 : 0).
// o = 0x5CE70(rq, f), null: 0. 0xD348(ECX o[0]); r2+0x4F clear: 0xD2FA(ECX o[0], EDX r2+0x3E), r2+0x80 == 1: 0xD330(ECX o[0], EDX
// 2*r2+0x54), 0xD2CA(ECX o[0], EDX r2+0x6C; r2+0x70) zero: 0. o[2] = rq; f & 4: 0xD33C(ECX o[0]; r2+0x4B ? 1.0f : 0). o[6] = -1,
// o[7] = o[3] = o[4] = o[5] = 0; 0x5D1D0 (EBX o). r2+0x4F clear: 0xD2E8(ECX o[0]) else 0x5DB30 (ECX rq+4; buf[80]) and 0xD342(ECX
// o[0], EDX buf; rq+0x24, r2+0x3E); a zero result clears o[2] and returns 0, else o.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c5d7a0G { uint32_t tbl; int32_t count; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_5d7a0(const D2cSvc& v, const D2c5d7a0G* g, uint32_t rq) {
    auto rec = [&](int32_t i) { return (g->tbl && i >= 0 && i < g->count) ? g->tbl + (uint32_t)i * 0x8e : 0u; };
    const uint32_t r1 = rec((int32_t)rd32(rq + 4));
    if (!(rd8(rq + 0x38) & 1)) wr32(rq + 4, d2c_call(v, 0x5ee70, 0, 0, 0, 0, 0, rd32(rq + 4)));
    const uint32_t nd = d2c_call(v, 0x5cc10, 0, 0, 0, 0, 0, rd32(rq + 4));
    if (nd) {
        const uint32_t en = rd32(nd + 8);
        if (!rd32(en + 0x3d) && rd32(rq + 0x34) - rd32(en + 0x34) < 2 && rd8(en + 0x3c) >= rd8(rq + 0x3c)) return 0;
    }
    const uint32_t r2 = rec((int32_t)rd32(rq + 4));
    wr32(r1 + 0x68, rd32(r1 + 0x64));
    wr32(r1 + 0x64, rd32(rq + 4));
    if (!rd8(r2 + 0x4f) && rd32(r2 + 0x86) != 2) {
        d2c_call(v, 0x5ebc0, rd8(r2 + 0x4d) == 0, rd32(rq + 4), 0, 0, 0, 0);
        if (rd32(r2 + 0x86) != 2) return 0;
    }
    if (rd8(r2 + 0x81)) return 0;
    uint32_t f = rd8(r2 + 0x50) ? 1 : 0;
    if (!(f & 1)) {
        if (d2c_call(v, 0xd324, 0, 0, 0, 0, 0) == 1) f |= 2;
        if (d2c_call(v, 0xd324, 0, 0, 0, 0, 0) == 2) f |= 6;
    }
    const uint32_t o = d2c_call(v, 0x5ce70, 0, 0, 0, 0, 0, rq, f);
    if (!o) return 0;
    d2c_call(v, 0xd348, 0, rd32(o), 0, 0, 0);
    if (!rd8(r2 + 0x4f)) {
        d2c_call(v, 0xd2fa, 0, rd32(o), rd8(r2 + 0x3e), 0, 0);
        if (rd8(r2 + 0x80) == 1) d2c_call(v, 0xd330, 0, rd32(o), rd32(r2 + 0x54) * 2, 0, 0);
        if (!d2c_call(v, 0xd2ca, 0, rd32(o), rd32(r2 + 0x6c), 0, 0, rd32(r2 + 0x70))) return 0;
    }
    wr32(o + 8, rq);
    if (f & 4) d2c_call(v, 0xd33c, 0, rd32(o), 0, 0, 0, rd8(r2 + 0x4b) ? 0x3f800000u : 0u);
    wr32(o + 0x18, 0xffffffffu); wr32(o + 0x1c, 0); wr32(o + 0xc, 0); wr32(o + 0x10, 0); wr32(o + 0x14, 0);
    d2c_call(v, 0x5d1d0, 0, 0, 0, o, 0);
    if (!rd8(r2 + 0x4f)) {
        if (!d2c_call(v, 0xd2e8, 0, rd32(o), 0, 0, 0)) { wr32(o + 8, 0); return 0; }
    } else {
        char buf[0x50] = {};
        const uint32_t b = (uint32_t)(uintptr_t)buf;
        d2c_call(v, 0x5db30, 0, rd32(rq + 4), 0, 0, 0, b);
        if (!d2c_call(v, 0xd342, 0, rd32(o), b, 0, 0, rd32(rq + 0x24), rd8(r2 + 0x3e))) { wr32(o + 8, 0); return 0; }
    }
    return o;
}
