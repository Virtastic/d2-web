// 1.13c D2Game+0x72220 -- ITEM_ApplyCubeRecipe: build the outputs of one CubeMain row and place them.
// stdcall (game, player, CubeMain record, inputs[3] {item, class, ilvl}), ret 0x10; returns output 0 or 0 when nothing was produced.
// Rule per output slot i (record row+0x4C+i*0x54): level from the fixed byte +9, or the player-level / input-ilvl percentages (+0xA/+0xB),
// clamped to [1, the max level]; the type byte +8 picks "use the input" (0xFE, or flag 1: a copy of the input item, 0x107E0), "item class"
// (0xFF/0xFC/0xFD: a fresh drop through 0x11490 with a 0x84-byte drop request), or a module special (1..3, the 0xFA5E8 table). A made item then
// loses its socketed children when the row removes inputs (flag 0x30), collects the input's socketed items when flag 0x20 asks, rolls the five
// mod slots (+0x18, chance byte +0xA against the item seed), applies repair/recharge (+1 & 2), the 0x400 callback (+1 & 4) and sockets
// (flag 2, capped by quality). Afterwards the player's cursor/inventory are refreshed and each made item is moved by GUID into its page
// (or dropped, 0xE20A0), with the hst / qf2 item-type callbacks; the collected socketed items follow the same placement.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <initializer_list>

struct D2Game72220Env {
    // every callee: rva, the registers it reads (eax ecx edx ebx esi edi), the stack words; returns EAX
    uint32_t (*call)(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi, const uint32_t* st, uint32_t n);
    uint32_t bitMasks;   // the array gdwBitMasks (D2Game pointer global 0xF8BE0) points at: 32 dwords 1<<i
    uint32_t invMasks;   // the array gdwInvBitMasks (0xF8BAC) points at: 32 dwords ~(1<<i)
    uint32_t mod;        // module base as the port sees module addresses (0xB2000000 in the probe)
};

namespace {
inline uint32_t r32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline uint32_t r16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
inline int32_t s16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
inline uint32_t r8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline void w32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
inline void w16(uint32_t a, uint32_t o, uint32_t v) { const uint16_t x = (uint16_t)v; std::memcpy((void*)(uintptr_t)(a + o), &x, 2); }
inline uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }

// the percentage helper inlined twice: v * p / 100 with the overflow guards (only the last branch is reachable for a byte p)
int32_t pct(int32_t v, int32_t p)
{
    if (v > 0x100000) return ((v & ~0xF) < 0x640) ? (int32_t)((int64_t)v * p / 100) : (v / 100) * p;
    if (p > 0x10000) return ((p & ~0xF) < 0x640) ? (int32_t)((int64_t)v * p / 100) : (p / 100) * v;
    return (int32_t)((uint32_t)v * (uint32_t)p) / 100;
}

enum : uint32_t {
    kStatGet = 0xA42A, kMaxLevel = 0xA48A, kSetOwner = 0xA92E, kSetPage = 0xA4A2, kCopy = 0x107E0, kClassFix = 0x11070, kClassPick = 0x71060,
    kDrop = 0x11490, kGetQ = 0xA898, kFileIdx = 0xA874, kGetIlvl = 0xA8F8, kRunes = 0xB37E, kAffA = 0x14210, kAffB = 0x14370, kSetQ = 0xB090,
    kSetPre = 0xB0AE, kSetSuf = 0xB0A8, kFlags = 0xA412, kSetFlag = 0xA65E, kB3C6 = 0xB3C6, kStatList = 0xB3F6, kStatRemove = 0xABB6,
    kStatFree = 0xABAA, kInvFirst = 0xA3DC, kInvNext = 0xA4CC, kInvItem = 0xA46C, kAddMod = 0xB09C, kHasDur = 0xA940, kMaxDur = 0xAA06,
    kSetStat = 0xA472, kHasType = 0xA8A4, kRecharge = 0xD5690, kStatBase = 0xA3E2, kCb400 = 0xD2380, kSockMax = 0xAC4C, kSetSock = 0xABE6,
    kPlrData = 0xA4C0, kRefresh = 0x711D0, kCursor = 0xA4DE, kClrCursor = 0xD29F0, kDropItem = 0xE20A0, kAnim = 0xA6C4, kPlaceRoom = 0xA634,
    kInvPlace = 0xAA9C, kInvAdd = 0xA9B8, kBodyLoc = 0xD4670, kIsEquip = 0xA640, kD47C0 = 0xD47C0, kInvUpd = 0xAA54, kA916 = 0xA916,
    kD1BC0 = 0xD1BC0, kA9A6 = 0xA9A6, kA994 = 0xA994, kBelt = 0x71B00, kEquipUpd = 0xD51A0, kItemTxt = 0xA53E, kHst = 0xC6CA0, kQf2 = 0xE4900,
};
constexpr uint32_t kTypeFn[4] = { 0, 0x72210, 0x70180, 0x70170 };   // the 0xFA5E8 table (module code words)
}

// 1.13c D2Game+0x72220
uint32_t D2Game_72220(uint32_t game, uint32_t unit, uint32_t row, uint32_t inputs, const D2Game72220Env& e)
{
    auto S = [&](uint32_t rva, std::initializer_list<uint32_t> st) { return e.call(rva, 0, 0, 0, 0, 0, 0, st.begin(), (uint32_t)st.size()); };
    auto R = [&](uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s, uint32_t di, std::initializer_list<uint32_t> st) {
        return e.call(rva, a, c, d, b, s, di, st.begin(), (uint32_t)st.size());
    };
    uint32_t out[3] = { 0, 0, 0 }, rem[3] = { 0, 0, 0 }, extras[64];
    uint32_t anyOut = 0, affixOk = 1, nExtra = 0;

    for (uint32_t i = 0; i < 3; i++) {
        const uint32_t rec = row + 0x4C + i * 0x54, inp = inputs + i * 12;
        out[i] = 0;
        if (r8(rec, 0) & 0x30) rem[i] = 1;
        int32_t lvl = 0;
        if (r8(rec, 9)) lvl = (int32_t)r8(rec, 9);
        else {
            if (const uint32_t p = r8(rec, 0xA)) lvl = pct((int32_t)S(kStatGet, { unit, 0xC, 0 }), (int32_t)p);
            if (const uint32_t p = r8(rec, 0xB)) lvl += pct((int32_t)r32(inp, 8), (int32_t)p);
        }
        const int32_t mx = (int32_t)S(kMaxLevel, { 0 });
        lvl = lvl > 1 ? lvl : 1;
        lvl = lvl < mx ? lvl : mx;
        uint32_t level = (uint32_t)lvl;
        const uint32_t t = r8(rec, 8);
        if (t && t < 4 && kTypeFn[t]) anyOut = R(kTypeFn[t], 0, game, unit, 0, 0, 0, {});
        const uint32_t flags = r16(rec, 0);

        if (flags & 1) {
            const uint32_t it = r32(inp, 0);
            S(kSetOwner, { it, 0xFF }); S(kSetPage, { it, 4 });
            const uint32_t dup = R(kCopy, game, 0, 0, 0, 0, 0, { it, rem[i] == 0 });
            uint32_t cls = 0;
            if (t == 0xFE) { const int32_t c = (int32_t)r32(inp, 4); if (c >= 0) cls = (uint32_t)c; }
            else if (t == 0xFF) cls = r32(inp, 4);
            else if (t == 0xFC) cls = r16(rec, 2);
            else if (t == 0xFD) cls = S(kClassPick, { game, level, r16(rec, 2) }) & 0xFFFF;
            if (dup) w32(dup, 4, cls);
            uint32_t slot = dup;
            R(kClassFix, 0, cls, 0, P(&slot), 0, 0, { game, 0, 0 });
            out[i] = dup;
            S(kSetPage, { dup, 4 }); S(kSetOwner, { it, 3 });
        } else if (t == 0xFE) {
            const uint32_t it = r32(inp, 0);
            S(kSetOwner, { it, 0xFF }); S(kSetPage, { it, 4 });
            const uint32_t dup = R(kCopy, game, 0, 0, 0, 0, 0, { it, rem[i] == 0 });
            out[i] = dup;
            S(kSetPage, { dup, 4 }); S(kSetOwner, { it, 3 });
            if (r8(rec, 6) == 9) {
                const uint32_t t1 = (S(kRunes, { dup }) & 0xFFFF) >= 1 ? S(kAffA, { dup, 1 }) : S(kAffB, { dup, 1 });
                const uint32_t t2 = (S(kRunes, { dup }) & 0xFFFF) >= 1 ? S(kAffA, { dup, 0 }) : S(kAffB, { dup, 0 });
                if ((t1 & 0xFFFF) && (t2 & 0xFFFF)) { S(kSetQ, { dup, 9 }); S(kSetPre, { dup, t1 }); S(kSetSuf, { dup, t2 }); }
                else affixOk = 0;
            }
        } else {
            uint32_t code = r16(rec, 4), q = r8(rec, 6), cls = 0;
            int32_t bitIdx = -1;
            bool go = true;
            if (t == 0xFF) {
                cls = r32(inp, 4);
                if (flags & 0x40) {
                    if (const uint32_t it = r32(inp, 0)) {
                        q = S(kGetQ, { it }) & 0xFF;
                        const int32_t b = (int32_t)S(kFileIdx, { it });
                        code = (uint32_t)b + 1;
                        level = S(kGetIlvl, { it });
                        if (q == 7 && !(r32(e.bitMasks, (uint32_t)(b & 31) * 4) & r32(game, 0x1B24 + (uint32_t)(b >> 5) * 4))) bitIdx = b;
                    }
                }
            } else if (t == 0xFC) cls = r16(rec, 2);
            else if (t == 0xFD) cls = S(kClassPick, { game, (uint32_t)lvl, r16(rec, 2) }) & 0xFFFF;
            else go = false;
            if (go) {
                uint32_t D[33];
                std::memset(D, 0, sizeof D);
                D[0] = unit; D[2] = game; D[3] = level; D[5] = cls; D[6] = 4;
                D[10] = 1u | (r16(game, 0x78) << 16);
                D[12] = q; D[16] = code & 0xFFFF;
                for (uint32_t j = 0; j < 6; j++) D[26 + j] = r16(rec, 0xC + 2 * j);
                const uint32_t lo = r8(rec, 0);
                D[32] = (((lo & 2) && !r8(rec, 7)) ? 0x10u : 8u) | ((lo & 4) ? 4u : 2u);
                out[i] = S(kDrop, { game, P(D), 0 });
                if (bitIdx >= 0) {
                    const uint32_t a = game + 0x1B24 + (uint32_t)(bitIdx >> 5) * 4;
                    w32(a, 0, r32(a, 0) & r32(e.invMasks, (uint32_t)(bitIdx & 31) * 4));
                }
            }
        }

        const uint32_t o = out[i];
        if (!o) continue;
        anyOut = 1;
        if (rem[i]) {
            if (r32(o, 0) == 4 && (S(kFlags, { o }) & 0x4000000)) {
                S(kSetFlag, { o, 0x4000000, 0 }); S(kB3C6, { o, 0, 0 });
                if (const uint32_t s = S(kStatList, { o, 0xAB, 0x40 })) { S(kStatRemove, { o, s }); S(kStatFree, { s }); }
            }
            if (r8(rec, 0) & 0x20) {
                const uint32_t it = r32(inp, 0);
                if (it && r32(it, 0x60)) {
                    for (uint32_t n = S(kInvFirst, { r32(it, 0x60) }); n;) {
                        const uint32_t nx = S(kInvNext, { n });
                        if (const uint32_t u = S(kInvItem, { n })) {
                            if (const uint32_t d = R(kCopy, game, 0, 0, 0, 0, 0, { u, 1 })) {
                                S(kSetOwner, { d, 0xFF }); S(kSetPage, { d, 4 });
                                if (nExtra >= 64) abort();
                                extras[nExtra++] = d;
                            }
                        }
                        n = nx;
                    }
                }
            }
        }
        if (affixOk) {
            for (uint32_t k = 0; k < 5; k++) {
                const uint32_t m = rec + 0x18 + k * 12;
                if ((int32_t)r32(m, 0) < 0) continue;
                const uint32_t c = r8(m, 0xA);
                if (c > 0 && c < 100) {
                    const uint64_t v = (uint64_t)r32(o, 0x20) * 0x6AC690C5u + r32(o, 0x24);
                    w32(o, 0x24, (uint32_t)(v >> 32)); w32(o, 0x20, (uint32_t)v);
                    if ((int32_t)((uint32_t)v % 100) > (int32_t)c) continue;
                }
                const uint32_t blk[4] = { r32(m, 0), (uint32_t)s16(m, 4), (uint32_t)s16(m, 6), (uint32_t)s16(m, 8) };
                S(kAddMod, { o, P(blk), r32(game, 0x70) });
            }
        }
        const uint32_t r7 = r8(rec, 7);
        if (r8(rec, 1) & 2) {
            if (S(kHasDur, { o }) && r7) { int32_t v = (int32_t)S(kMaxDur, { o }); if (v >= (int32_t)r7) v = (int32_t)r7; S(kSetStat, { o, 0x46, (uint32_t)v, 0 }); }
            if (S(kHasType, { o, 0x100, 0x501, e.mod + 0xFBD3C })) R(kRecharge, 0, 0, 0, 0, unit, 0, { game });
            else {
                const int32_t full = (int32_t)S(kStatGet, { o, 0x49, 0 });
                if ((int32_t)S(kStatBase, { o, 0x48, 0 }) < full) S(kSetStat, { o, 0x48, (uint32_t)full, 0 });
            }
        }
        if (r8(rec, 1) & 4) R(kCb400, game, unit, 0, 0, 0, 0, { o });
        if (r8(rec, 0) & 2) {
            if (!r7) continue;
            if (S(kStatGet, { o, 0xC2, 0 })) continue;
            if (S(kFlags, { o }) & 0x800) continue;
            int32_t s = (int32_t)(S(kSockMax, { o }) & 0xFF);
            if (s >= (int32_t)r7) s = (int32_t)r7;
            const uint32_t qv = S(kGetQ, { o }) - 4;
            if (qv <= 5) { const int32_t cap = (qv == 0 || qv == 5) ? 3 : 1; if (s > cap) s = cap; }
            if (s > 0) { S(kSetFlag, { o, 0x800, 1 }); S(kSetSock, { o, (uint32_t)s }); }
        } else if (r7 && S(kHasDur, { o })) {
            int32_t v = (int32_t)S(kMaxDur, { o }); if (v >= (int32_t)r7) v = (int32_t)r7;
            S(kSetStat, { o, 0x46, (uint32_t)v, 0 });
        }
    }

    if (!anyOut) return 0;
    const uint32_t client = (unit && r32(unit, 0) == 0) ? r32(S(kPlrData, { unit }), 0x9C) : 0;
    R(kRefresh, unit, client, 0, 0, 0, 0, { game });
    if (!unit) abort();   // retail: assert line 0x558, exit(-1)
    w16(unit, 0x6E, 4); w32(unit, 0x70, unit);
    S(kCursor, { unit });
    w32(unit, 0xC4, r32(unit, 0xC4) | 0x400);

    auto place = [&](uint32_t o, bool extra) {
        S(kSetOwner, { o, 3 });
        const uint32_t g = r32(o, 0xC);
        uint32_t node = 0;
        if (game) {
            S(kClrCursor, { unit });
            for (uint32_t n = r32(game, 0x1720 + (g & 0x7F) * 4); n; n = r32(n, 0xE4)) if (r32(n, 0xC) == g) { node = n; break; }
        }
        uint32_t inv = 0, doUpdate = 1, doRefresh = 0;
        bool ok = false;
        if (node && r32(node, 0) == 4 && r32(node, 0x10) == 4) {
            const uint32_t pg = S(kAnim, { node }) & 0xFF;
            inv = r32(unit, 0x60);
            if (inv) {
                const uint32_t r = S(kPlaceRoom, { unit, pg, r32(game, 0x70) });
                if (r32(unit, 0) == 0) { if (pg == 1) doUpdate = 0; else if (pg == 2) doRefresh = 1; }
                ok = S(kInvPlace, { inv, node, r, 0, pg, e.mod + 0xFBD3C, extra ? 0x579u : 0x558u }) && S(kInvAdd, { inv, node, 1 });
            }
        }
        if (!ok) { R(kDropItem, 0, game, o, 0, 0, 0, {}); return; }
        if ((S(kAnim, { node }) & 0xFF) != 4) R(kBodyLoc, node, unit, 0, 0, 0, 0, {});
        w32(node, 0xC4, r32(node, 0xC4) & ~2u);
        if (S(kIsEquip, { node, unit })) S(kD47C0, { 0 });
        if (doUpdate) S(kInvUpd, { inv, 0 });
        S(kSetPage, { node, 0 }); S(kA916, { node, 2, 1 });
        if (R(kD1BC0, node, 0, 0, 0, 0, 0, {})) S(kSetFlag, { node, 1, 1 });
        if (S(kHasType, { node, 0x4000, 0x8EF, e.mod + 0xFB14C })) S(kSetFlag, { node, 0x4000, 0 });
        S(kA9A6, { unit, 1 }); S(kA994, { inv, node });
        if (doRefresh == 1) R(kBelt, node, game, 0, 0, 0, 0, { unit });
        if (S(kIsEquip, { node, unit })) { S(kEquipUpd, { game, unit, 0 }); S(kA9A6, { unit, 1 }); }
        S(kSetFlag, { o, 0x10, 1 });
        if (extra) return;
        const uint32_t txt = S(kItemTxt, { r32(o, 4) });
        if (r8(txt, 0x12A)) {
            const uint32_t code = r32(txt, 0x80);
            if (code == 0x20747368) R(kHst, 0, 0, 0, game, 0, unit, {});        // "hst "
            else if (code == 0x20326671) R(kQf2, unit, game, 0, 0, 0, 0, {});   // "qf2 "
        }
    };
    for (uint32_t k = 0; k < 3; k++) if (out[k]) place(out[k], false);
    for (uint32_t k = 0; k < nExtra; k++) if (extras[k]) place(extras[k], true);
    return out[0];
}
