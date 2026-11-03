// D2Game 1.13c 0x5EED0 (A5Q4 kill/event handler), written from the retail disassembly. Same Env rule as
// d2game_batch_5d5c0.cpp: every callee is one E::h(rva, ...) with register arguments first, then the stack arguments.
// ECX = quest node, EDX = event message. Not linked into a build yet.
#include <cstdint>

namespace b5eed0 {
typedef uint32_t u32;

template <class E> void A5Q4Event(E& e, u32 q, u32 msg)
{
    u32 st = e.rd32(q + 0x18);
    if (!e.rd8(st + 0x11) || !e.rd8(st + 0x10) || e.rd32(st + 0xC)) return;
    if (!e.rd8(st + 4)) { e.wr8(st + 4, 1); e.h(0x33AF0, 2, q, 0x5E660); }       // EBX = 2, stack (q, callback)
    u32 tgt = e.rd32(msg + 8);
    u32 type = tgt ? e.rd32(tgt + 4) : 0xFFFFFFFFu;
    u32 id;
    if (type == 0x21C) { id = e.rd32(st + 0x1C); e.wr8(st + 0x3E, 1); }
    else if (type == 0x21D) { id = e.rd32(st + 0x14); e.wr8(st + 0x3C, 1); }
    else if (type == 0x21E) { id = e.rd32(st + 0x18); e.wr8(st + 0x3D, 1); }
    else return;
    e.h(0x5D960, id, q, msg);                                                     // EBX = id, stack (q, msg)
    u32 game = e.rd32(q + 4);
    e.wr8(e.rd32(game + 0x10F4) + 0x20, 0x12);
    e.h(0xDFCC0, 0, 0x359C0, game, 0);                                           // EAX = 0, EBX = callback, EDI = game, stack 0
    e.wr32(st + 0x44, e.rd32(st + 0x44) - 1);
    if (e.rd32(st + 0x44) != 0) return;
    e.wr8(st, 1);
    if (!e.rd8(q + 9)) return;
    u32 who = e.rd32(msg + 0xC);
    if (who) {
        u32 pl = e.h(0x1020, e.rd32(q + 4));
        if (e.h(0xA568, pl, 0x27, 0) == 0) {
            u32 n = e.h(0xA3E2, who, 0xC, 0);
            if ((int32_t)n >= (int32_t)((e.rd8(e.rd32(q + 4) + 0x6D) + 1) * 20)) {
                e.h(0xA57A, pl, 0x27, 0);
                e.h(0xA57A, pl, 0x27, 0xD);
                e.h(0x5BF70, who, e.rd32(msg));                                   // EAX = who, stack msg[0]
            }
        }
    }
    u32 g = e.rd32(msg), obj = e.rd32(msg + 8);
    e.h(0xDFCC0, 0, 0x5C100, g, obj);                                            // EDI = game, stack obj
    e.h(0xDFCC0, 0, 0x58F40, g, obj);
    e.h(0x35EB0, 5);
    u32 room = e.h(0xA478, obj);
    u32 r1130 = e.h(0x1130, obj);                                                // ECX = obj
    u32 r1100 = e.h(0x1100, obj);
    u32 pool = e.rd32(g + 0x1C);
    u32 u = e.h(0xA6B8, pool, 2);
    if (u) {
        e.wr32(u, 0, 2);
        e.wr32(u + 4, 0x231);
        e.wr32(u + 8, pool);
        e.wr32(u + 0x80, g);
        u32 f = e.rd32(u + 0xC8);
        f = e.rd32(g + 0x70) ? (f | 0x2000000) : (f & 0xFDFFFFFFu);
        e.wr32(u + 0xC8, f | 0x4000000);
        u32 b = e.h(0xA56E, e.h(0xA556, room)) & 0xFF;
        e.wr8(u + 0x18, b);
        e.wr32(u + 0x1C, e.rd32(g + 0xBC + b * 4));
        e.wr32(u + 0x74, 0);
        e.h(0xE0110, g + 0xD0, u);                                               // ESI = &game[0xd0], EDI = unit
        e.wr32(u + 0xC4, e.rd32(u + 0xC4) | 0x10);
        e.wr32(u + 0xD0, 0xB);
        u32 c = e.rd32(g + 0x98) + 1;
        if (c == 0xFFFFFFFFu) c = 1;
        e.wr32(g + 0x98, c);
        u32 m = e.h(0xA2E6, pool, 0x38, 0x265, 0);                               // ECX pool, EDX 0x38, stack file / 0x265 / 0
        e.wr32(u + 0x14, m);
        for (u32 i = 0; i < 14; i++) e.wr32(m + i * 4, 0);
        e.wr32(m, e.h(0xA520, 0x231));
        e.wr32(u + 0x10, 0);
        e.h(0xA724, u);
        e.h(0x2F300, u, c, room, r1100, r1130);                                  // ESI = unit, stack (counter, room, r1100, r1130)
        e.h(0xE0F40, room, r1130, u, r1100, g, 1);                               // EAX = room, ECX = r1130, EDX = unit
    }
    if (u32 d = e.h(0x33F30, e.rd32(q + 4), e.rd32(q + 0x10))) {                  // EAX = game, ECX = q[0x10]
        u32 fn = e.rd32(d + 0xF0);
        if (e.h(0x1D18D14, fn)) { u32 s2 = e.h(0xA2DA, 0x357); e.h(0xA2C2, 0, s2); e.h(0x2A3C, 0xFFFFFFFFu); }
        e.h(fn, d);                                                              // thiscall [d + 0xF0], ECX = d
    }
    if (e.rd8(q + 0xB) != 0xD) {
        e.wr8(q + 0x14, 0);
        e.wr8(q + 0xB, 0xD);
        e.h(0xDFCC0, 0, 0x58F90, e.rd32(q + 4), 0);
    }
}
}  // namespace b5eed0
