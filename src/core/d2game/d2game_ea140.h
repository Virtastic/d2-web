// 1.13c D2Game+0xEA140: object operate callback "orb" (table entry). ecx = the operate block. An object past
// its neutral mode: 0. The player's cursor item (0xA54A: inventory at player +0x60) must have the code "qf2 "
// (0xA532: item), else the player only hears the sound 0x13 (0xDFFD0: esi = player; stack 0x13, player). The
// quest record 0x13's data counts the hit at +0x38; below 2: 0. Then quest 0x12 bits 0 and 0xD are set in the
// player's quest flags (player data 0xA4C0, word +0x10 + 4 * the game's difficulty byte +0x6D; 0xA57A), a
// held "qf2 " (0xE660: eax = player, ebx = game; stack: code) is taken away (0x365E0: ecx = player; stack:
// game, inventory), and with quest 0x15 bit 4 set but bit 0 clear in the flags read through 0x1020 (eax =
// player; stack: game), bit 0 is set there. The game must exist (fatal line 0x2D3). The unit whose id is data
// +0x2C is looked up in the game's table (+0x1320 + 4 * (id & 0x7F), next +0xE4, id +0x0C); found: 0xF21E0
// (eax 0; stack: game, 0, 0) and 0xEAB60 (ebx = the unit; stack: game). The object goes to mode 1 (0xA4A2)
// and, with its class record (0xA520: class, -1 without an object), the mode timer is queued (0x8EA80: ecx =
// game, edx = object; 1, frame + (record +0xDC >> 8), 0, 0, 0). Data byte +0x0C = 1, the state change is sent
// (0x35E90: eax = the record's game +4), the record's handler +0xF0 is called (ecx = record; a bad code
// pointer is fatal line 0x353), and a player in a party (0xDFA60: eax = player, a word other than 0xFFFF) has
// the party visited (0x9C160: ebx 0, esi = game; stack: callback 0xE87E0). The result is 0.
// handler = the hook id the record's handler slot answers to in the probe; 0 reads it from the slot.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"
#include "d2game_fatal.h"

namespace gea140 {
template <class E> uint32_t Operate(E& e, uint32_t op, uint32_t handler = 0, uint32_t mod = 0xB2000000u)
{
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 0;
    const uint32_t cur = e.h(0xA54A, 0, 0, 0, 0, 0, e.rd32(e.rd32(op + 8) + 0x60));
    if (!cur || e.h(0xA532, 0, 0, 0, 0, 0, cur) != 0x20326671) {
        const uint32_t pl = e.rd32(op + 8);
        e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
        return 0;
    }
    const uint32_t q = gobjinit::Find(e, op, 0x13);
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t n = e.rd32(d + 0x38) + 1;
    e.wr32(d + 0x38, n);
    if ((int32_t)n < 2) return 0;
    const auto flags = [&] { const uint32_t g = e.rd32(op); return e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8)) + e.rd8(g + 0x6D) * 4 + 0x10); };
    { const uint32_t f = flags(); e.h(0xA57A, 0, 0, 0, 0, 0, f, 0x12, 0); }
    { const uint32_t f = flags(); e.h(0xA57A, 0, 0, 0, 0, 0, f, 0x12, 0xD); }
    const uint32_t pl = e.rd32(op + 8);
    uint32_t game = e.rd32(op);
    if (e.h(0xE660, pl, 0, 0, game, 0, 0x20326671)) e.h(0x365E0, 0, pl, 0, 0, 0, game, e.rd32(pl + 0x60));
    { const uint32_t f = flags();
      if (e.h(0xA568, 0, 0, 0, 0, 0, f, 0x15, 4)) {
          const uint32_t f2 = e.h(0x1020, e.rd32(op + 8), 0, 0, 0, 0, e.rd32(op));
          if (!e.h(0xA568, 0, 0, 0, 0, 0, f2, 0x15, 0)) {
              const uint32_t f3 = e.h(0x1020, e.rd32(op + 8), 0, 0, 0, 0, e.rd32(op));
              e.h(0xA57A, 0, 0, 0, 0, 0, f3, 0x15, 0);
          }
      } }
    game = e.rd32(op);
    const uint32_t id = e.rd32(d + 0x2C);
    if (!game) { d2gfatal::Fatal(e, 0x2D3, mod); return 0; }
    for (uint32_t u = e.rd32(game + (id & 0x7F) * 4 + 0x1320); u; u = e.rd32(u + 0xE4))
        if (e.rd32(u + 0xC) == id) {
            e.h(0xF21E0, 0, 0, 0, 0, 0, game, 0, 0);
            e.h(0xEAB60, 0, 0, 0, u, 0, e.rd32(op));
            break;
        }
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
    const uint32_t o2 = e.rd32(op + 4);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, o2 ? e.rd32(o2 + 4) : 0xFFFFFFFFu);
    const uint32_t g = e.rd32(op);
    e.h(0x8EA80, 0, g, e.rd32(op + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(g + 0xA8), 0, 0, 0);
    e.wr8(d + 0xC, 1);
    e.h(0x35E90, e.rd32(q + 4));
    e.h(handler ? handler : (e.rd32(q + 0xF0) & 0x00FFFFFFu), 0, q);
    if ((e.h(0xDFA60, e.rd32(op + 8)) & 0xFFFF) == 0xFFFF) return 0;
    e.h(0x9C160, 0, 0, 0, 0, e.rd32(op), 0xB20E87E0u);
    return 0;
}
}
