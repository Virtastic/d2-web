// src/core/d2game/d2game_batch_65e90b.cpp -- 1.13c D2Game unit, trade, packet and quest-spawn rows transcribed
// from the cached Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Same convention as
// d2game_batch_65e90.cpp: E supplies rd8/rd16/rd32/wr8/wr32 over the 32-bit retail address space and the callee
// seams in retail argument order. Host scratch buffers (u32*) stand for retail stack records the callee fills.
#include <cstdint>

namespace d2game_b65e90b {
typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  s32;

// 1.13c D2Game+0x65E90
// Relink spawned units: for each id in p2's table (count at +0x88, ids from +0x68) that is still on the
// game's unit hash, leave the unit's position and reclaim it. p1[1] is the game. Uses the sub_21280 and
// sub_EF650 seams; sub_A0000 returns the unit to hand the relink to.
template <class E> void QuestRelinkSpawnedUnits(E& e, u32 p1, u32 p2)
{
    u32 game0 = e.rd32(p1 + 4);
    if (!(e.rd8(p2 + 0x118) != 0 && e.rd8(p2 + 0x119) == 0)) return;
    s32 found = 0;
    s32 c = 0;
    if (e.rd16(p2 + 0x88) == 0) return;
    u32 ids = p2 + 0x68;
    do {
        if (game0 == 0) { e.fatal(0x2D3); return; }
        u32 id = e.rd32(ids);
        for (u32 it = e.rd32(game0 + 0x1520 + (id & 0x7F) * 4); it != 0; it = e.rd32(it + 0xE4)) {
            if (e.rd32(it + 0xC) != id) continue;
            u32 pos[2] = { e.rd32(p2 + 0x110), e.rd32(p2 + 0x114) };
            bool brk = false;
            if (found == 0) {
                e.ord10331(it);
                s32 r = e.sub_21280();
                if (r == 0 || (r = e.sub_EF650(0x146, 0xC, game0, (u32)r, pos[0], pos[1], 0xFFFFFFFFu, 8)) == 0) {
                    e.ord10141(it, pos);
                    c += 5;
                    e.ord10331(it);
                    r = e.sub_21280();
                    r = e.sub_EF650(0x146, 0xC, game0, (u32)r, pos[0], pos[1], 0xFFFFFFFFu, 8);
                    if (r == 0) brk = true;
                }
            }
            if (brk) break;
            e.wr8(p2 + 0x119, 1);
            u32 type = e.rd32(it);
            if (type != 2 && (type < 4 || type > 5) && e.rd32(it + 0x2C) != 0) e.ord10750(e.rd32(it + 0x2C));
            type = e.rd32(it);
            if (type != 2 && (type < 4 || type > 5) && e.rd32(it + 0x2C) != 0) e.ord10867(e.rd32(it + 0x2C));
            u32 nu = e.sub_A0000(game0);
            if (nu != 0) {
                e.ord10637(nu, id);
                e.ord10019(nu, 0);
                e.ord10346(e.ord10331(nu), 0);
            }
            break;
        }
        c += 1;
        ids += 4;
    } while (c < (s32)e.rd16(p2 + 0x88));
}

// 1.13c D2Game+0x68FC0
// Spawn the quest monster if it is absent. p[0] = game, p[1] = unit, p[2] = room/source, p[4] = target.
// Scans the game's quest list (+0x10F4) for a node of type 4. A missing node or a node that is live and not
// flagged as spawned gets the spawn: mode 1 on the unit, a 0x54 = 3 flag on the node, and the trailing
// packets. Otherwise the fail path sends 0x13 through sub_DFFD0.
template <class E> u32 SpawnQuestMonsterIfAbsent(E& e, u32 p)
{
    u32 game = e.rd32(p);
    u32 list = e.rd32(game + 0x10F4);
    u32 node = 0;
    if (list != 0) {
        for (u32 n = e.rd32(list); n != 0; n = e.rd32(n + 0xF4)) {
            if (e.rd32(n) == 4) { node = n; break; }
        }
    }
    u32 st = 0;
    if (node == 0 || (st = e.rd32(node + 0x18), e.rd8(node + 9) != 0 && e.rd8(st + 0x50) != 1 && e.rd8(node + 0xC) < 6)) {
        u32 room = e.ord10920(e.rd32(p + 8));
        u32 slot = e.rd32(room + 0x10 + (u32)e.rd8(game + 0x6D) * 4);
        s32 r = e.ord10174(slot, 4, 1);
        if (r == 0 && (r = e.ord10174(slot, 4, 0)) == 0) {
            u32 info = e.ord10688(e.rd32(p + 0x10));
            u32 unit = e.rd32(p + 4);
            if (unit == 0 || e.rd32(unit + 0x10) == 0) {
                e.ord11090(unit, 1);
                e.sub_8EA80(game, e.rd32(p + 4), 1, (e.rd32(info + 0xDC) >> 8) + e.rd32(game + 0xA8), 0, 0, 0);
                if (st != 0) {
                    e.wr32(st + 0x54, 3);
                    e.wr32(st + 0x3C, e.rd32(p + 8) == 0 ? 0xFFFFFFFFu : e.rd32(e.rd32(p + 8) + 0xC));
                }
                e.sub_8EA80(game, e.rd32(p + 4), 7, e.rd32(game + 0xA8) + 0x11, 0, 0, 0);
                e.ord10346(e.ord10331(e.rd32(p + 4), 0));
                e.ord10003(slot, 4, 0xD);
                e.ord10003(slot, 4, 1);
                e.sub_54C00();
                if (e.sub_DFA60() != -1) e.sub_9C160(0x629B0);
            }
            return 0;
        }
        e.sub_DFFD0(0x13, e.rd32(p + 8));
    }
    return 0;
}

// 1.13c D2Game+0x6AD60
// Walk the room's unit list (+0x74) and give each live, non-self unit the trade-or-drop treatment of its type.
// Type 0 and 1 units go through sub_142D0 when their class fits; a type 2 unit of class 0xB with no state gets
// the sub_D740 record when its ord10522 distance is under 3. obj[0] = game, obj[1] = unit.
template <class E> u32 IterateNearbyQuestTargets(E& e, u32 obj)
{
    u32 self = e.rd32(obj + 4);
    e.ord10141();
    u32 room = e.ord10331(self);
    for (u32 u = e.rd32(room + 0x74); u != 0; u = e.rd32(u + 0xE8)) {
        u32 type = e.rd32(u);
        if (u == self || type == 5) continue;
        if (type == 0) {
            if (e.rd32(u + 0x10) != 0x11 && e.ord10714(u, self, 3) != 0) e.sub_142D0(e.rd32(obj), self, u);
        } else if (type == 1) {
            if (e.rd32(u + 0x10) != 0xC && e.ord10714(u, self, 3) != 0) e.sub_142D0(e.rd32(obj), self, u);
        } else if (type == 2 && u != 0 && e.rd32(u + 4) == 0xB && e.rd32(u + 0x10) == 0) {
            if (e.ord10522(self, u) < 3) {
                u32 rec[1] = { u };   // REG?: retail passes &local_1c; the record layout past the first word is unpinned
                e.sub_D740(rec);
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0x6B430
// Spawn the rotating code items: for each marked code item on the room list (+0x60) that is not "non ",
// sub_6B070 places it; with none placed, the rotation state at p[2]+0x20 (64-bit) picks one of six colour
// codes ("cgw ", "cgr ", "cg g", "cgb ", "cgy ", "cgv ") and sub_6B070 places it. Fatal 0x823 / 0x84C on a lookup miss.
template <class E> void SpawnRotatingCodeItems(E& e, u32 p1, u32 p1b)
{
    (void)p1b;
    u32 uVar6 = e.rd32(e.rd32(p1 + 8) + 0x60);
    u32 it = e.ord10010(uVar6, 0x14, 0);
    bool placed = false;
    if (it != 0) {
        do {
            if (placed) return;
            u32 id = (it == 0) ? 0xFFFFFFFFu : e.rd32(it + 4);
            u32 rec = e.ord10695(id);
            if (e.rd32(rec + 0xBC) != 0x206E6F6Eu) {
                s32 code = e.ord10814(e.rd32(rec + 0xBC));
                if (code == -1) { e.fatal(0x823); return; }
                if (e.sub_6B070(e.rd32(p1), e.rd32(p1 + 8), (u32)code, it) != 0) { placed = true; it = 0; }
            }
            it = e.ord10010(uVar6, 0x14, it);
        } while (it != 0);
        if (placed) return;
    }
    u32 st = e.rd32(p1 + 8);
    {
        u64 v = (u64)e.rd32(st + 0x20) * 0x6AC690C5ull + (u64)e.rd32(st + 0x24);
        e.wr32(st + 0x20, (u32)v);
        e.wr32(st + 0x24, (u32)(v >> 32));
        u32 code = 0;
        switch ((u32)v % 6u) {
        case 0: code = 0x20776367u; break;
        case 1: code = 0x20726367u; break;
        case 2: code = 0x20676367u; break;
        case 3: code = 0x20626367u; break;
        case 4: code = 0x20796367u; break;
        default: code = 0x20766367u; break;
        }
        s32 id = e.ord10814(code);
        if (id != -1) { e.sub_6B070(e.rd32(p1), e.rd32(p1 + 8), (u32)id, 0); return; }
        e.fatal(0x84C);
    }
}

// 1.13c D2Game+0x6B8A0
// Move a unit along its path point: the x/y pair comes from the target unit's (or its record's) +0x10 and +0xC,
// then ord10314 starts the move with the sub_6A440 callback. On failure the sub_24B70 seam runs on the game.
template <class E> void MoveUnitToPathPoint(E& e, u32 p1)
{
    u32 pu = e.rd32(p1 + 8);
    s32 t = (s32)e.rd32(pu);
    u32 x, y;
    if (t == 2 || (t > 3 && t < 6)) x = e.rd32(e.rd32(pu + 0x2C) + 0x10);
    else if (e.rd32(pu + 0x2C) == 0) x = 0;
    else x = e.ord10750(e.rd32(pu + 0x2C));
    pu = e.rd32(p1 + 8);
    t = (s32)e.rd32(pu);
    if (t == 2 || (t > 3 && t < 6)) y = e.rd32(e.rd32(pu + 0x2C) + 0xC);
    else if (e.rd32(pu + 0x2C) == 0) y = 0;
    else y = e.ord10867(e.rd32(pu + 0x2C));
    s32 r = e.ord10314(e.rd32(p1 + 8), y, x, 0, e.sub_6A440_cb());
    if (r != 0) e.sub_24B70(e.rd32(p1));
}

// 1.13c D2Game+0x6BCB0
// Return-to-town idle on mode 2: a mode-0 unit is set to mode 2; on mode 2 the room's unit list must hold a
// type-5 unit (else fatal 0x40E) and sub_E1C00 runs for the game. Returns 1 either way.
template <class E> u32 ReturnToTownIdleOnMode2(E& e, u32 p1)
{
    u32 unit = e.rd32(p1 + 4);
    u32 pos[2] = { 0, 0 };
    e.ord10331(unit);
    e.ord10141(unit, pos);
    e.ord10589(unit);
    e.ord10904(unit);
    u32 iVar2 = e.rd32(p1 + 4);
    u32 m = (iVar2 == 0) ? 0 : e.rd32(iVar2 + 0x10);
    if (m == 0) {
        e.ord11090(iVar2, 2);
    } else if (m == 2) {
        u32 room = e.ord10331(iVar2);
        u32 u = e.rd32(room + 0x74);
        for (;;) {
            if (u == 0) { e.fatal(0x40E); return 1; }
            if (e.rd32(u) == 5) break;
            u = e.rd32(u + 0xE8);
        }
        e.sub_E1C00(e.rd32(p1));
        return 1;
    }
    return 1;
}

// 1.13c D2Game+0x6FFC0
// Push a trade node on the player's trade list (+0x08 of the trade record at +0x5C). Register inputs: ESI (the
// item), EAX (the owner); the node is allocated with the PlrTrade.cpp tag and line 0xC58. Node: [0]=esi,
// [1]=eax, [2]=old head.
template <class E> void PLRTRADE_AddItemTradePair(E& e, u32 esi, u32 eax)
{
    u32 pl = e.ord10920();
    if (pl != 0) {
        u32 rec = e.rd32(pl + 0x5C);
        if (rec != 0) {
            u32 node = e.alloc_tagged("..\\Source\\D2Game\\PLAYER\\PlrTrade.cpp", 0xC58, 0);
            e.wr32(node, esi);
            e.wr32(node + 4, eax);
            e.wr32(node + 8, e.rd32(rec + 8));
            e.wr32(rec + 8, node);
        }
    }
}

// 1.13c D2Game+0x75070
// Initialise a 0x20-byte state record: fatal 0x7B8 when the index is above 0xF; zero it, set the index and the
// unit at +4, and set the byte at +0x15 to 0x65 when sub_74F20 says so, else 100.
template <class E> void PLRSTATE_InitRecord(E& e, u32 index, u32 esi, u32 edi)
{
    if (index > 0xF) { e.fatal(0x7B8); return; }
    e.ord11103();
    for (u32 i = 0; i < 8; i++) e.wr32(esi + i * 4, 0);
    e.wr32(esi, index);
    e.wr32(esi + 4, edi);
    u8 v = 100;
    if (e.sub_74F20(edi) != 0) v = 0x65;
    e.wr8(esi + 0x15, v);
}

// 1.13c D2Game+0x75350
// Clear a unit's state and sound: stop the sound handle at +0xA4 (sets flag 0x100 on the unit's +0xC4), resolve
// the state record through the unit's +0x14 (type 1), run the sub_41040 / sub_410D0 pair on the state's flag
// bits, clear the table record's bits, and finish with the sub_13B0 / ord10706 hand-off. EAX = unit.
template <class E> void UNIT_ClearStateAndSound(E& e, u32 p1, u32 unit)
{
    u32 st = 0;
    if (unit != 0) {
        if (e.rd32(unit + 0xA4) != 0) {
            e.ord10422(e.rd32(p1 + 0x1C), e.rd32(unit + 0xA4));
            e.wr32(unit + 0xA4, 0);
            e.ord10158();
            e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x100);
        }
        if (e.rd32(unit) == 1 && e.rd32(unit + 0x14) != 0) {
            st = e.rd32(e.rd32(unit + 0x14) + 0x28);
            goto have_state;
        }
    }
    st = 0;
have_state:
    if ((e.rd16(st + 8) & 1) != 0) {
        if ((e.rd16(st + 8) & 2) == 0) e.sub_41040();
        else e.sub_410D0(unit);
    }
    u32 id = (unit == 0) ? 0 : 0;
    if (unit != 0) id = e.rd32(unit + 4);
    u32 rec = 0;
    if (!((unit == 0) || (s32)id < 0 || (s32)id >= (s32)e.tableCount(0xA80))) rec = id * 0x1A8 + e.tableBase(0xA78);
    e.sub_EFC60();
    if (unit != 0) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFF7FF3u);
    e.ord11004();
    e.ord10064(rec);
    e.ord10627();
    if (e.ord10341() != 0 && unit != 0) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) & 0xFFFFFFFDu);
    u32 cl = e.rd32(unit + 0x2C);
    if (e.sub_13B0(unit, 0x13) == 0) e.ord10084();
    e.ord10706(cl, unit);
    e.sub_AF8D0();
}

// 1.13c D2Game+0x755C0
// Broadcast nearby state packets: each unit on the game's room list (+0x74, type 0, not type 0x11, within
// ord10522 distance 3 and not yet sent) gets a 0x1C-word record built, then sub_C0B0 / sub_E0C0 / sub_CF20 send
// it. p1 = game; EBX (game) is the register input.
template <class E> void GAME_BroadcastNearbyStatePackets(E& e, u32 p1, u32 ebx)
{
    u32 room = e.ord10331();
    for (u32 u = e.rd32(room + 0x74); u != 0; u = e.rd32(u + 0xE8)) {
        if (e.rd32(u) == 0 && e.rd32(u + 0x10) != 0x11 && e.ord10522() < 3 && e.ord10324(u) == 0) {
            u32 rec[0x1C] = { 0 };
            e.ord10973(u, 6, 0);
            e.ord10494(u, 0x36);
            e.sub_DC0B0(p1, ebx, u, rec);
            e.sub_DE0C0(p1, ebx, 1, rec);
            e.sub_DCF20(p1, ebx, rec);
        }
    }
}

// 1.13c D2Game+0x760F0
// Clear a skill's unit effects: EAX = unit with the flag at +0xC8 bit 10 set; its skill record comes from sub_E03A0
// (keyed by +0x98) and must sit inside the table; the record's objects are removed, the class-2 effects of its
// list (+0xDC) are sent through sub_8E6F0; if the hit test fails, sub_DEEE0 runs instead.
template <class E> void SKILL_ClearUnitEffects(E& e, u32 p1, u32 eax)
{
    if (eax == 0 || ((e.rd32(eax + 0xC8) >> 10) & 1) == 0) return;
    u32 r = e.sub_E03A0(p1, e.rd32(eax + 0x98));
    if (r == 0) return;
    s32 id = (s32)e.rd32(r + 4);
    if (!(id > -1 && id < (s32)e.tableCount(0xA80) && (u32)id * 0x1A8 + e.tableBase(0xA78) != 0)) return;
    u32 t = e.rd32(r);
    if (t != 2 && (t < 4 || t > 5) && e.rd32(r + 0x2C) != 0) e.ord10867(e.rd32(r + 0x2C));
    t = e.rd32(r);
    if (t != 2 && (t < 4 || t > 5) && e.rd32(r + 0x2C) != 0) e.ord10750(e.rd32(r + 0x2C));
    u32 sv = e.ord10589(r, 0x3C01);
    sv = e.ord10331(r, sv);
    s32 a = e.ord10509(sv);
    s32 b = e.ord10830();
    if (b == 0 && a != 0 && e.sub_12380(r) != 0) {
        u32 n = e.rd32(r + 0xDC);
        if (n != 0) {
            do {
                u8 kind = e.rd8(n);
                n = e.rd32(n + 0x24);
                if (kind == 2) e.sub_AE6F0();
            } while (n != 0);
        }
    } else {
        e.sub_DEEE0(0, r, 0, 1);
    }
}

}
