// src/core/d2game/d2game_batch_65e90.cpp -- 1.13c D2Game quest-state, drain, and table rows transcribed from the
// cached Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Memory is 32-bit retail address space
// through E::rd8/rd16/rd32/wr8/wr32; callees are seams in retail argument order. Ordinal_NNNNN names are the
// D2Common/D2Game imports as the decompile labels them; sub_<RVA> names are D2Game-internal callees.
// Nothing here is called from the play path yet: each function is a transcription awaiting its wire-in.
#include <cstdint>

namespace d2game_b65e90 {
typedef uint8_t  u8;
typedef uint32_t u32;
typedef int32_t  s32;

// 1.13c D2Game+0x68250
// Env: sub_DFCC0(ecx, arg) -> void ; rd32/wr8.
template <class E> u32 ClearFlag46ReturnTrue(E& e, u32 ecx, u32 p2)
{
    e.sub_DFCC0(ecx, 0);
    u32 st = e.rd32(p2 + 0x18);
    e.wr8(st + 0x46, 0);
    return 1;
}

// 1.13c D2Game+0x68280
// __thiscall: this = ecx (passed through to the sub_DFCC0 seam), p1 = quest obj, p2/p3 are the two flags.
// Env: rd8/rd32/wr8/wr32; ord11090(unit, mode); sub_8EA80 is not used here; sub_EC40(0x92); sub_53AF0(p1);
//      sub_DFCC0(this, 0); fatal(code) never returns.
template <class E> void ApplyQuestNpcStateChange(E& e, u32 thisp, u32 p1, u8 p2, u8 p3)
{
    u32 q = e.rd32(p1 + 0x18);
    bool skip54 = false;
    if (e.rd8(q + 0x48) == 1) {
        thisp = e.rd32(q + 0x34);
        if (e.rd32(p1 + 4) == 0) { e.fatal(0x2D3); return; }
        for (u32 it = e.rd32(e.rd32(p1 + 4) + 0x1520 + (thisp & 0x7F) * 4); it != 0; it = e.rd32(it + 0xE4)) {
            if (e.rd32(it + 0xC) == thisp) {
                if (e.rd32(it + 4) == 0x1A) {
                    thisp = e.rd32(it + 0x10);
                    if (thisp == 3) { skip54 = true; break; }
                    e.ord11090(it, 3); // REG?: retail's extraout_ECX after this call is not modelled; thisp keeps the state value
                }
                break;
            }
        }
    }
    if (!skip54) e.wr32(q + 0x54, 3);
    if (e.rd8(q + 0x47) == 1) {
        thisp = e.rd32(q + 0x30);
        if (e.rd32(p1 + 4) == 0) { e.fatal(0x2D3); return; }
        for (u32 it = e.rd32(e.rd32(p1 + 4) + 0x1520 + (thisp & 0x7F) * 4); it != 0; it = e.rd32(it + 0xE4)) {
            if (e.rd32(it + 0xC) == thisp) { e.ord11090(it, 1); break; }
        }
    }
    if (p2 == 0) {
        if (p3 == 0) { e.wr8(q + 0x50, 1); return; }
        e.sub_DFCC0(thisp, 0);
    } else if (e.rd8(q + 0x46) == 0) {
        e.wr8(q + 0x46, 1);
        e.sub_33AF0(p1);
    }
    if (p3 != 0) {
        if (e.rd8(q + 0x61) == 0) e.sub_1EC40(0x92);
        if (e.rd8(q + 0x51) == 0) e.wr8(q + 0x52, 1);
    }
    e.wr8(q + 0x50, 1);
}

// 1.13c D2Game+0x68F90
// __fastcall: p = ecx; p[0] = game, p[1] = unit.
template <class E> void RaiseQuestNodeFlagAndSend(E& e, u32 p)
{
    u32 game = e.rd32(p), unit = e.rd32(p + 4);
    e.ord11090(unit, 1);
    e.sub_8EA80(game, unit, 7, e.rd32(game + 0xA8) + 0x19, 0, 0, 0);
}

// 1.13c D2Game+0x697A0
// Drain mana into life: the stat-8 value is cut by p2[1]% and p2[2]% of it is added to stat 6.
template <class E> void DrainManaIntoLife(E& e, u32 p1, u32 p2)
{
    u32 unit = e.rd32(p1 + 8);
    u32 cur = (u32)(e.rd32(p2 + 4) * (s32)e.ord10973(unit, 8, 0)) / 100;
    e.ord10551(unit, 8, (u32)(0u - cur), 0);
    e.ord10551(unit, 6, (e.rd32(p2 + 8) * cur) / 100, 0);
}

// 1.13c D2Game+0x69800
// Drain life into mana: stat 6 is kept in its high byte (cur >> 8) and the same percentages move it to stat 8.
template <class E> void DrainLifeIntoMana(E& e, u32 p1, u32 p2)
{
    u32 unit = e.rd32(p1 + 8);
    s32 raw = e.ord10973(unit, 6, 0);
    u32 cur = (u32)(e.rd32(p2 + 4) * (raw >> 8)) / 100;
    e.ord10551(unit, 6, cur * 0xFFFFFF00u, 0);
    e.ord10551(unit, 8, (e.rd32(p2 + 8) * cur * 0x100u) / 100, 0);
}

// 1.13c D2Game+0x69940
// REG?: EAX = in_EAX (the unit the caller passed in the register); item class 0xEA needs a non-zero EAX and
// ord10459 of the ord10826 result. Returns 0 only for that refused case.
template <class E> u32 CheckItemClassEA(E& e, u32 param1, u32 eax)
{
    if (param1 == 0xEA && eax != 0) {
        u32 v = e.ord10826(eax);
        if (e.ord10459(v) == 0) return 0;
    }
    return 1;
}

// 1.13c D2Game+0x69A80
// p[1] is the owner unit. An unowned minion (or one with state 0) gets ord11090(owner, 1) and loses bit 1 of
// +0xC4 on the unit; a live owner with state < 3 gets ord11090(owner, 0).
template <class E> u32 ClearMinionFlagIfUnowned(E& e, u32 p)
{
    u32 owner = e.rd32(p + 4);
    if (owner == 0 || e.rd32(owner + 0x10) == 0) {
        e.ord11090(owner, 1);
        if (e.rd32(p + 4) != 0) {
            u32 a = e.rd32(p + 4) + 0xC4;
            e.wr32(a, e.rd32(a) & 0xFFFFFFFDu);
        }
    } else if (e.rd32(owner + 0x10) < 3) {
        e.ord11090(owner, 0);
        return 1;
    }
    return 1;
}

// 1.13c D2Game+0x69DE0
// Clamp HP to max; when it was below max, the byte at esi is set to 1. States 2 and 1 are cleared from the unit.
// Ends in the sub_DC920 seam.
template <class E> void ClampHpToMaxAndClearStates(E& e, u32 unit, u32 esi)
{
    s32 hp = e.ord10973(unit, 6, 0);
    s32 mx = e.ord10913(unit);
    if (hp < mx) {
        e.ord10887(unit, 6, (u32)mx, 0);
        e.wr8(esi, 1);
    }
    u32 st = e.ord10871(unit, 2);
    if (st != 0) { e.ord11111(unit, st); e.ord11108(st); }
    st = e.ord10871(unit, 1);
    if (st != 0) { e.ord11111(unit, st); e.ord11108(st); }
    e.sub_DC920();
}

// 1.13c D2Game+0x6A050
// REG?: EBX = event index, ESI = obj (obj+4 = unit). Index must lie in the data table; 0xEA needs a live unit
// whose ord10459 says yes; then sub_EDF20 is tried up to three times (short-circuit on non-zero).
template <class E> void DispatchIndexedQuestEvent(E& e, s32 ebx, u32 esi)
{
    if (ebx < 0 || ebx >= (s32)e.tableCount(0xA80)) return;
    u32 unit = e.ord10331(e.rd32(esi + 4));
    if (ebx == 0xEA && unit != 0) {
        u32 v = e.ord10826(unit);
        if (e.ord10459(v) == 0) return;
    }
    s32 r = e.sub_EDF20();
    if (r == 0) {
        r = e.sub_EDF20();
        if (r == 0) e.sub_EDF20();
    }
}

// 1.13c D2Game+0x770A0
// Scaled table value for level eax; the level table is retail data behind the scaleTable seam.
// Returns 0 when the record's flag at +0x6D is clear or the level is 1 or lower.
template <class E> u32 LEVEL_ScaledTableValue(E& e, u32 p1, u32 eax)
{
    if (e.rd8(p1 + 0x6D) != 0 && (s32)eax > 1) {
        if (eax < 9) return e.scaleTable(eax);
        return eax * 8 - 0x10;
    }
    return 0;
}
}
