// 1.13c D2Game+0xDBD90 -- unit damage tail for monsters (ECX = unit, EDX = value, one unused stack word); written from the 1.13c disassembly.
// A "plain" unit is set, has bit 0 of byte +0xC6 clear and, by type (+0), has +0x10 nonzero and not 0x11 (type 0) or not 0xC (type 1). A unit that is not plain runs
// 0xA7E4(unit, value) first; a nonzero result is the return. Then 0xA3CA(unit, value, 0), 0xDA750(owner = unit +0x80; EDI unit) and 0xA4A2(unit, 1).
// A type 1 unit with an owner (+0x80) and a data block (the first word of the block at +0x14) takes a recovery amount from byte +0x4F of that word (indexed by owner +0x6D
// unless owner +0x6A and +0x74 are both clear; 0 becomes 15), runs 0x8E6F0(node; EDI owner) for each kind 2 node of the chain at unit +0xDC (next at +0x24) and returns
// 0x8EA80(ECX owner, EDX unit; 2, owner +0xA8 + amount, 0, 0, 0). A null unit (exit path of retail) is not modelled.
#include "../fn/hookenv.h"
namespace d2g {
static bool plain_dbd90(HookEnv& e, uint32_t u)
{
    if (!u || (e.rd8(u + 0xC6) & 1)) return false;
    const uint32_t t = e.rd32(u), k = e.rd32(u + 0x10);
    if (t == 0) return k != 0 && k != 0x11;
    if (t == 1) return k != 0 && k != 0xC;
    return false;
}
uint32_t fdbd90(HookEnv& e, uint32_t u, uint32_t v)
{
    if (!plain_dbd90(e, u)) {
        const uint32_t r = e.h(0xA7E4, 0, 0, 0, 0, 0, u, v);
        if (r) return r;
    }
    e.h(0xA3CA, 0, 0, 0, 0, 0, u, v, 0);
    e.h(0xDA750, 0, 0, 0, 0, 0, e.rd32(u + 0x80));
    e.h(0xA4A2, 0, 0, 0, 0, 0, u, 1);
    const uint32_t t = e.rd32(u);
    if (t != 1) return t;
    const uint32_t ow = e.rd32(u + 0x80);
    if (!ow) return 1;
    const uint32_t blk = e.rd32(u + 0x14);
    if (!blk) return 0;
    const uint32_t base = e.rd32(blk);
    if (!base) return 0;
    uint32_t amt = (e.rd8(ow + 0x6A) || e.rd32(ow + 0x74)) ? e.rd8(base + e.rd8(ow + 0x6D) + 0x4F) : e.rd8(base + 0x4F);
    if (!amt) amt = 0xF;
    for (uint32_t n = e.rd32(u + 0xDC); n;) {
        const uint32_t nx = e.rd32(n + 0x24);
        if (e.rd8(n) == 2) { e.h(0x8E6F0, n); e.edi(0x8E6F0, ow); }
        n = nx;
    }
    const uint32_t r = e.h(0x8EA80, 0, ow, u, 0, 0, 2, e.rd32(ow + 0xA8) + amt, 0, 0, 0);
    const uint32_t z = 0;
    e.x(0x8EA80, &z, 1);
    return r;
}
}
