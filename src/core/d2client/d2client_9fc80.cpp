// 1.13c D2Client+0x9FC80 -- stdcall(msg): msg+0x18 = 1, +0x1C = 0, Storm 511(msg). UI mode m = [0x10B214]: other than 1 or 2 returns m.
// Mode 2: a = player ? Ordinal 0xC202([player+0x60]) : 0; [0x11B834] = a, [0x11B858] = 0, [0x11B854] = 5, [0x11B864] = a ? 4 : 1.
// Modes 1 and 2: [0x10B210] == 0 sends the 17-byte request {0x44, id (player+0xC or -1), [0x10B21C], 0, 2} to FUN 0x143E0 (EBX = 0x11),
// then FUN 0x9F9E0(); otherwise FUN 0x48150() then FUN 0x9F9E0(). The return is the 0x9F9E0 result.
#include <cstring>
#include "cl_cmd.h"
extern "C" uint32_t d2c_9fc80(uint32_t msg, const cl::Env& e) {
    e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
    uint32_t m = clc::G(e, 0x10B214);
    uint32_t pl = clc::G(e, 0x11BBFC);
    if (m == 2) {
        uint32_t a = 0;
        if (pl) { a = e.H(0xc202, e.R(pl + 0x60)); pl = clc::G(e, 0x11BBFC); }
        clc::S(e, 0x11B834, a); clc::S(e, 0x11B858, 0); clc::S(e, 0x11B854, 5); clc::S(e, 0x11B864, a == 0 ? 1u : 4u);
    } else if (m != 1) return m;
    if (clc::G(e, 0x10B210) == 0) {
        uint32_t id = pl ? e.R(pl + 0xc) : 0xffffffffu, g = clc::G(e, 0x10B21C), z = 0, two = 2;
        uint8_t tmp[20] = {}; tmp[0] = 0x44;
        memcpy(tmp + 1, &id, 4); memcpy(tmp + 5, &g, 4); memcpy(tmp + 9, &z, 4); memcpy(tmp + 13, &two, 4);
        volatile uint8_t blk[20];
        for (int i = 0; i < 20; i++) blk[i] = tmp[i];
        uint32_t pb = (uint32_t)(uintptr_t)blk;
        e.H(0x143e0, pb, 0, 0, 0, 0, 0, 0, 0, 0x11);
        if (e.blk) e.blk(e.ctx, 0x143e0, pb, 5);
    } else e.H(0x48150);
    return e.H(0x9f9e0);
}
