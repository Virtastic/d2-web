// 1.13c D2Client+0x578F0 -- queue the skill-0xad cast action for unit ESI at its path spot plus a class offset.
// ESI = unit, no stack args, returns EAX = 0xB30F0 result or 0. Callee seam: D2c73Env (registers per the disassembly).
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2client_578f0(const D2c73Env* e, uint32_t self) {
    const uint32_t path = rd32(e, self + 0x2c);
    if (!path || !C(e, 0xc352, 0, 0, 0, 0, 0, 0, path)) return 0;
    const uint32_t h = C(e, 0xc838, 0, self, 0xad);                 // ecx = unit, edx = 0xad, no stack args
    const uint32_t lvl = C(e, 0xc718, 0, 0, 0, 0, 0, 0, self, h, 1);
    const uint32_t cell = C(e, 0xc1f0, 0, 0, 0, 0, 0, 0, self);
    const uint32_t t = rd32(e, self);
    const bool direct = (t == 2 || (t >= 4 && t <= 5));
    uint32_t x = direct ? rd32(e, path + 0xc) : C(e, 0xc3be, 0, 0, 0, 0, 0, 0, path);
    uint32_t y = direct ? rd32(e, path + 0x10) : C(e, 0xc2ec, 0, 0, 0, 0, 0, 0, path);
    uint8_t k[4] = {0, 0, 0, 0};
    static const uint8_t tab[8] = {0xa, 0x8, 0x16, 0x14, 0x12, 0x10, 0xe, 0xc};
    const uint32_t cls = C(e, 0xcc88, 0, 0, 0, 0, 0, 0, C(e, 0xc6f4, 0, 0, 0, 0, 0, 0, path)) & 0xFF;
    k[0] = tab[cls & 7]; k[1] = tab[1]; k[2] = tab[2]; k[3] = tab[3];   // retail passes the dword with byte 0 replaced
    uint32_t dw; memcpy(&dw, k, 4);
    int32_t d[2] = {0, 0};
    C(e, 0xcc64, 0, 0, 0, 0, 0, 0, dw, ptr(&d[0]), ptr(&d[1]));
    x += (uint32_t)d[0]; y += (uint32_t)d[1];
    if (C(e, 0xc208, 0, 0, 0, 0, 0, 0, C(e, 0x1400, cell, 0, 0, x, 0, y))) return 0;
    uint32_t rec[23] = {};
    rec[0] = 0x25; rec[1] = self; rec[4] = 0x8f; rec[5] = x; rec[6] = y;
    rec[7] = x + (uint32_t)d[0] * 2; rec[8] = y + (uint32_t)d[1] * 2; rec[10] = 3; rec[12] = lvl;
    const uint32_t ret = C(e, 0xb30f0, 0, ptr(rec));
    e->blk(e->ctx, 0xb30f0, rec, 23);   // the capture's `hookblk` record of the request block
    C(e, 0x88a70, 0, 0, 0, 0x726, 0, 0, self, 0, 0, 0);
    return ret;
}
