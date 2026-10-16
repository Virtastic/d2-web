// 1.13c D2Client+0x164F0: one tick of the 7-entry UI animation state machine.
// Callees 0x160E0 (frames<<8) and 0x16130 (entry+4) are inlined; their fatal-exit guards return 1.
#include <cstdint>
// tbl: the 7 x 0x1c-byte table at D2Client+0xF8558, tbl[49] = the count at +0xF861C.
// rng: *(+0x11BBFC) (seed lo +0x20, hi +0x24). st: +0x11B854 idx, +0x11B858 acc, +0x11B860 tick, +0x11B864 state.
extern "C" uint32_t d2_client_164f0(const uint32_t* tbl, uint32_t* rng, uint32_t* st) {
    uint32_t& idx = st[0]; uint32_t& acc = st[1]; uint32_t& state = st[4];
    const uint32_t i = idx;
    if (!rng) return 0;
    if (state == 1) {
        uint64_t v = (uint64_t)rng[8] * 0x6ac690c5u + rng[9];
        rng[8] = (uint32_t)v; rng[9] = (uint32_t)(v >> 32);
        if (((uint32_t)v & 0x3f) < 0x10) acc += 0x20;
    } else acc += tbl[i * 7 + 3];
    if (tbl[49] != 7 || (int32_t)i >= 7) return 1;
    if ((int32_t)acc < (int32_t)(tbl[i * 7 + 2] * 0x100)) return 0;   // signed: the retail cmp/jl
    if (tbl[i * 7 + 1]) { acc -= tbl[i * 7 + 2] << 8; return 0; }
    if (state == 2) { state = 4; idx = 3; acc = 0; }
    else if (state == 5) { state = 1; idx = 5; acc = 0; }
    else return 1;
    return 0;
}
