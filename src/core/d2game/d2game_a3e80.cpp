// 1.13c D2Game+0xA3E80: score a candidate unit for the AI think. Written from the
// disassembly: the decompile drops EAX/ECX inputs and the pushed constants. Divisions are exact signed (magic constants
// 0x88888889 = /15, 0x2e8ba2e9 = /22). The 0xA49C null-record read (record index out of range) faults in retail; here the byte is 0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_a3e80.h"

typedef uintptr_t A;
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint8_t rd8(A p, uint32_t o) { return ((const uint8_t*)p)[o]; }

static uint32_t weaponByte(const D2GameA3E80Env* e, A item, uint32_t line)
{
    int32_t idx = e->fA49C(item, line);
    if (idx < 0 || idx >= (int32_t)rd32(e->dt, 0xBA0)) return 0;
    return rd8((A)((uint32_t)idx * 0x23C + rd32(e->dt, 0xB98)), 0x18E);
}

// 1.13c D2Game+0xA3E80
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_game_A3E80(A other, A unit, A t, const D2GameA3E80Env* e)
{
    uint32_t r754 = e->fA754(t, unit, 0);
    int32_t dist = 0;
    if (other) {
        int32_t x = (int32_t)rd32(other, 0xC), y = (int32_t)rd32(other, 0x10);
        if (e->fB0980(x, y) > 0x55 && e->fB0980(x, y) < 0x55) dist = 0x64;
    }
    uint32_t s24 = e->getStat(unit, 0x24, 0), s27 = e->getStat(unit, 0x27, 0), s29 = e->getStat(unit, 0x29, 0), s25 = e->getStat(unit, 0x25, 0);
    uint32_t s2b = r754 ? e->getStat(unit, 0x2B, 0) : 0;
    int32_t q = (int32_t)(s2b + 4 * s24 + 8 * s25 + s29 + s27) / 15;
    uint32_t s16 = e->getStat(unit, 0x16, 0), s31 = e->getStat(unit, 0x31, 0), s33 = e->getStat(unit, 0x33, 0), s35 = e->getStat(unit, 0x35, 0),
             s37 = e->getStat(unit, 0x37, 0), s3a = e->getStat(unit, 0x3A, 0);
    int32_t h = (int32_t)(((uint32_t)((int32_t)s3a >> 8)) + s37 + s35 + s33 + s31 + s16) / 2;
    uint32_t st = e->fA68E(unit, 0xB) ? 100 : 0;
    uint32_t lvl = e->fA718(unit) >= 0x14 ? 0 : 100;
    uint32_t w1 = 0, w2 = 0, b1 = 0, b2 = 0;
    if (A it = e->fA3AC(unit)) { w1 = (uint32_t)e->fA400(unit, it, 1); b1 = weaponByte(e, it, 0x225); }
    if (A it = e->fA3B2(unit)) { w2 = (uint32_t)e->fA400(unit, it, 1); b2 = weaponByte(e, it, 0x22B); }
    int32_t p = (int32_t)(b1 * w1 + b2 * w2) / 4;
    uint32_t d = r754 ? 100 : (e->fAD78(t, unit, 4) ? 0 : 0x4B);
    if (unit && rd32(unit, 0) == 1) {
        int32_t cid = (int32_t)rd32(unit, 4);
        if (cid < 0 || cid >= (int32_t)rd32(e->dt, 0xA80)) return 0;
        A row = (A)((uint32_t)cid * 0x1A8 + rd32(e->dt, 0xA78));
        if (!row || rd8(row, 0x4E) <= 1) return 0;
    }
    int32_t total = q + 5 * (dist + (int32_t)d) + 2 * (p + 2 * (int32_t)st + h) + 3 * (int32_t)lvl;
    uint32_t res = (uint32_t)(total / 22);
    return res ? res : 1;
}
