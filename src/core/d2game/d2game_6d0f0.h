// 1.13c D2Game+0x6D0F0 (a player takes a town portal / red portal): ecx = S {+0 game, +4 portal, +8 player, +0x10 kind}, always returns 0.
// Written from the 1.13c disassembly as a rule over an environment E: every callee that is not a field read is one E::h(rva, ...) call; `now` is GetTickCount.
// Gates in order: a player that is a type-0 unit with +0x6C set, 0xA6A6(+0x60) nonzero or player-data +0x4C set leaves; 0x37AC0(player, game) nonzero needs the portal owner (0xB168 +4) to be the player;
// the 5 s re-entry stamp (player data +0x160, else the +0x38 list's node keyed -1 at +8) refuses (0xDFFD0 0x13); an owner who is not the player must share the player's 0xDFA60 id (the game's id chain at +0x1120).
// A paired portal (0xE1570, its first word 2) supplies the destination point and room, else the level's warp point (0xA56E act, 0xABA4 into the point); 0xAEC8 gets 0xAEE0|5 (warp path: as the low byte of the x the lookup wrote).
// The move (0xA694 placement, 0xE19C0), the client notices (0xDFFD0 8, 0x78550, 0x8C280), the paired-portal close or the 0x8EA80 state item, and the final 0x4B+game[0xA8] item (0xA6A0 ... 0xA60A).
// The not-driven fatals (type != 0 player, null game in the id chain, 0xE19C0 returning 0) trap.
#pragma once
#include <cstdint>

namespace r6d0f0 {
template <class E> uint32_t Run(E& e, uint32_t S, uint32_t now)
{
    const uint32_t game = e.rd32(S), portal = e.rd32(S + 4), pl = e.rd32(S + 8);
    auto uid = [&](uint32_t u) { return u ? e.rd32(u + 0xC) : 0xFFFFFFFFu; };
    auto utype = [&](uint32_t u) { return u ? e.rd32(u) : 6u; };
    auto fail = [&]() { e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl); return 0u; };
    auto A478 = [&](uint32_t u) { return e.h(0xA478, 0, 0, 0, 0, 0, u); };
    auto A44E = [&](uint32_t room, uint32_t id) { e.h(0xA44E, 0, 0, 0, 0, 0, room, 2, id); };
    auto A51A = [&](uint32_t room) { e.h(0xA51A, 0, 0, 0, 0, 0, room, 1); };
    if (pl != 0 && e.rd32(pl) == 0) {
        if (e.rd8(pl + 0x6C) != 0) return 0;
        if (e.h(0xA6A6, 0, 0, 0, 0, 0, e.rd32(pl + 0x60)) != 0) return 0;
        if (e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, pl) + 0x4C) != 0) return 0;
    }
    if (e.h(0x37AC0, pl, game) != 0) {
        const uint32_t own = e.h(0xB168, 0, portal);
        if (uid(pl) != (own ? e.rd32(own + 4) : 0xFFFFFFFFu)) return 0;
    }
    uint32_t stamp = 0;
    if (const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl)) {
        stamp = e.rd32(pd + 0x160);
    } else if (pl != 0) {
        for (uint32_t n = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, pl) + 0x38); n != 0; n = e.rd32(n + 0x10))
            if (e.rd32(n) == 0xFFFFFFFFu) { stamp = e.rd32(n + 8); break; }
    }
    if (stamp + 5000u > now) return fail();
    {
        const uint32_t own = e.h(0xB168, 0, portal);
        const uint32_t t = own ? e.rd32(own + 4) : 0xFFFFFFFFu;
        if (t != 0xFFFFFFFFu && t != uid(pl)) {
            if ((uint16_t)e.h(0xDFA60, pl) == 0xFFFF) return fail();
            if (game == 0) __builtin_trap();                  // retail: fatal line 0x2D3
            for (uint32_t p = e.rd32(game + 0x1120 + (t & 0x7F) * 4); p != 0; p = e.rd32(p + 0xE4))
                if (e.rd32(p + 0xC) == t) {
                    const uint16_t a = (uint16_t)e.h(0xDFA60, p);
                    if ((uint16_t)e.h(0xDFA60, pl) != a) return fail();
                    break;
                }
        }
    }
    if (pl == 0 || e.rd32(pl) != 0) __builtin_trap();         // retail: fatal line 0xED8
    uint32_t pair = e.h(0xE1570, 0, 0, 0, 0, 0, game, portal);
    if (pair != 0 && e.rd32(pair) != 2) pair = 0;

    if (A478(portal) != 0) {
        const uint32_t ebp = e.h(0x1020, pl, 0, 0, 0, 0, game);
        const uint32_t esi = ebp ? e.h(0xB144, 0, e.rd8(e.rd32(portal + 0x14) + 4)) : 0;
        if (ebp == 0 || esi == 0) return fail();
        if (e.rd32(S + 0x10) == 0x3B) {
            const uint32_t a = e.h(0x37440, pl);
            if (a != uid(portal) && pair != 0 && a != e.rd32(pair + 0xC)) {
                const int32_t v = (int32_t)(e.rd32(game + 0x70) == 0 ? e.rd32(esi) : e.rd32(esi + 4));
                if (v > 0 && e.h(0xA568, 0, 0, 0, 0, 0, ebp, (uint32_t)v, 0) == 0) return fail();
            }
        }
    }
    uint32_t pt[2] = {0, 0}, room;
    if (pair != 0) {
        pt[0] = e.h(0x1100, 0, pair);
        pt[1] = e.h(0x1130, 0, pair);
        room = A478(pair);
    } else {
        const uint32_t lvl = e.rd8(e.rd32(portal + 0x14) + 4);
        const uint8_t act = (uint8_t)e.h(0xA56E, 0, 0, 0, 0, 0, lvl);
        room = e.h(0xABA4, 0, 0, 0, 0, 0, e.rd32(game + 0xBC + act * 4u), lvl, 0, e.stk(&pt[0]), e.stk(&pt[1]), 3);
    }
    {   // a byte is stored into a stack slot and the whole dword pushed: the warp path's slot holds the x the lookup wrote, the paired path's is stale stack (the descriptor masks it)
        const uint32_t f = (uint8_t)(e.h(0xAEE0, 0, 0, 0, 0, 0, portal) | 5);
        e.h(0xAEC8, 0, 0, 0, 0, 0, portal, pair != 0 ? f : ((pt[0] & 0xFFFFFF00u) | f));
    }
    if (e.h(0xA808, 0, 0, 0, 0, 0, A478(pl)) != 0) {
        const uint32_t a = e.h(0xA556, 0, 0, 0, 0, 0, room);
        const uint32_t b = e.h(0xA556, 0, 0, 0, 0, 0, A478(pl));
        e.h(0x34860, game, b, a, 0, 0, pl);
    }
    const uint32_t from = e.h(0xA71E, 0, 0, 0, 0, 0, pl);
    const uint32_t room2 = e.h(0xA694, 0, 0, 0, 0, 0, room, e.stk(pt), from, 0x1C09, 0);
    if (room2 == 0) return 0;
    const uint32_t x = pt[0], y = pt[1];                      // read after the placement, which may move the point
    if (e.h(0xE19C0, y, x, room2, 0, game, game, pl, 0, 0) == 0) __builtin_trap();   // retail: fatal line 0xF52
    const uint32_t client = (e.rd32(pl) == 0) ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, pl) + 0x9C) : 0;
    e.h(0xDFFD0, 0, 0, 0, 0, pl, 8, 0);
    e.h(0x78550, 0, game, pl, 0, 0, 0, 2, x + 5, y + 5, 0);
    e.h(0x8C280, utype(pl), (client & 0xFFFFFF00u) | 0xD, uid(pl), 0, 0, client, 1, x + 5, y + 5, 0, 0);
    const uint32_t cls = portal ? e.rd32(portal + 4) : 0xFFFFFFFFu;
    if (pair == 0 || (uint16_t)cls == 0x3C) {
        if (portal != 0 && e.rd32(portal + 0x10) == 1) {
            const uint32_t row = e.h(0xA520, 0, 0, 0, 0, 0, cls & 0xFFFF);
            e.h(0x8EA80, 0, game, portal, 0, 0, 1, (e.rd32(row + 0xDC) >> 8) + 1 + e.rd32(game + 0xA8), 0, 0, 0);
        }
    } else if (e.h(0x37440, pl) == e.rd32(pair + 0xC)) {
        uint32_t r = A478(portal);
        A44E(r, uid(portal));
        e.h(0xE20A0, 0, game, portal);
        A51A(r);
        e.h(0x56740, pair, 0, 0, 0, 0, game);
        r = A478(pair);
        A44E(r, e.rd32(pair + 0xC));
        e.h(0xE20A0, 0, game, pair);
        A51A(r);
    }
    const uint32_t st = e.rd32(game + 0xA8) + 0x4B;
    const uint32_t it = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 2, st, 0, utype(pl));
    if (it != 0) {
        e.h(0x8EA80, 0, game, pl, 0, 0, 0xC, st, 0, 0, 0);
        e.h(0xA3CA, 0, 0, 0, 0, 0, pl, 0x66, 1);
        e.h(0xA5B6, 0, 0, 0, 0, 0, it, 0x66);
        e.h(0xA75A, 0, 0, 0, 0, 0, it, 0x6FCBF7A0u);
        e.h(0xA60A, 0, 0, 0, 0, 0, pl, it, 1);
    }
    return 0;
}
}
