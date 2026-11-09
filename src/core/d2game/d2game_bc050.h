// 1.13c D2Game+0xBC050: create the class-0x83 world object at a free spot near (x, y).
// EAX = self {x, y}; stack ctx {game, unit}, mode (ret 8, the mode only picks a dead local). Written from the 1.13c disassembly as a rule over an
// environment E (hookenv.h): every callee that is not a field read is one E::h(rva, ...) call.
// room = 0xA478(unit); 0x21280 (EAX = room, EBX = x, EDI = y) finds the room covering (x, y), null returns. 0x34130(room, self, 3, 0x3F11,
// &v, 100) writes the target room v through its 5th argument, v == 0 returns. Then the SUnit allocation as in 0x6BF00 (class 0x83, +0x10 = 0,
// no +0x78), 0x2F300(esi unit; id, v, x, y), 0xE0F40(eax v, ecx y, edx unit; x, game, 1), 0xA51A(v, 0), 0xBBF30(ecx ctx).
#pragma once
#include <cstdint>

namespace rbc050 {
template <class E> void CreateObject(E& e, uint32_t self, uint32_t ctx)
{
    const uint32_t game = e.rd32(ctx), unitIn = e.rd32(ctx + 4);
    const uint32_t x = e.rd32(self), y = e.rd32(self + 4);
    const uint32_t room0 = e.h(0xA478, 0, 0, 0, 0, 0, unitIn);
    e.ed(y);
    const uint32_t room = e.h(0x1280, room0, 0, 0, x);
    e.edi(0x1280, y);
    if (!room) return;
    uint32_t v = 0;
    e.h(0x34130, 0, 0, 0, 0, 0, room, self, 3, 0x3F11, e.stk(&v), 100);
    if (!v) return;
    const uint32_t pool = e.rd32(game + 0x1C);
    const uint32_t u = e.h(0xA6B8, 0, 0, 0, 0, 0, pool, 2);
    if (!u) return;
    e.wr32(u + 0, 2); e.wr32(u + 4, 0x83); e.wr32(u + 8, pool); e.wr32(u + 0x80, game);
    uint32_t fl = e.rd32(u + 0xC8);
    fl = e.rd32(game + 0x70) ? (fl | 0x2000000u) : (fl & 0xFDFFFFFFu);
    e.wr32(u + 0xC8, fl); e.wr32(u + 0xC8, fl | 0x4000000u);
    const uint32_t act = e.h(0xA56E, 0, 0, 0, 0, 0, e.h(0xA556, 0, 0, 0, 0, 0, v)) & 0xFF;
    e.wr8(u + 0x18, act);
    e.wr32(u + 0x1C, e.rd32(game + 0xBC + act * 4));
    e.wr32(u + 0x74, 0);
    e.ed(u); e.h(0xE0110, 0, 0, 0, 0, game + 0xD0); e.edi(0xE0110, u);
    e.wr32(u + 0xC4, e.rd32(u + 0xC4) | 0x10);
    e.wr32(u + 0xD0, 0xB);
    uint32_t id = e.rd32(game + 0x98) + 1;
    if (id == 0xFFFFFFFFu) id = 1;
    e.wr32(game + 0x98, id);
    const uint32_t ud = e.h(0xA2E6, 0, e.rd32(game + 0x1C), 0x38, 0, 0, 0xB20FB02Cu, 0x265, 0);
    e.wr32(u + 0x14, ud);
    for (uint32_t i = 0; i < 14; i++) e.wr32(ud + 4 * i, 0);
    e.wr32(e.rd32(u + 0x14), e.h(0xA520, 0, 0, 0, 0, 0, 0x83));
    e.wr32(u + 0x10, 0);
    e.h(0xA724, 0, 0, 0, 0, 0, u);
    e.h(0x2F300, 0, 0, 0, 0, u, id, v, x, y);
    e.h(0xE0F40, v, y, u, 0, 0, x, game, 1);
    e.h(0xA51A, 0, 0, 0, 0, 0, v, 0);
    e.h(0xBBF30, 0, ctx);
}
}
