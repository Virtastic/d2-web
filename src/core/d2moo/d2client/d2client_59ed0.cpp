// 1.13c D2Client+0x59ED0 -- the server 0x77 "UI action" handler (EAX = packet; the opcode stub 0xAF930 is `mov eax, ecx; jmp`).
// Byte 1 of the packet selects one of 22 actions; above 0x15 nothing.
// gl layout (offset: module RVA), every global the body reads or writes:
//   +0x00 0x11C2CC trade state      +0x04 0x11C2D0 (byte) panel-dirty  +0x0C 0x113A68 busy         +0x10 0x11BBFC player
//   +0x14 0x123498                  +0x18 0x12349C                     +0x1C 0x11C434
//   +0x20 0x1177E4 partner id       +0x28 0x1177EC my gold (u16 pair) +0x2C 0x1177F0 wait box    +0x30 0x1177F4  +0x38 0x1177FC
//   +0x3C 0x10B348                  +0x40 0x11BE5C partner mark        +0x44 0x11BE60 my mark      +0x58 0x11BE74 blocked
//   +0x5C 0x11BC08                  +0x60 0x11971D interact unit       +0x64 0x119721 interact flag
//   +0x70 0xFAD80 open-screen flags (0x26 dwords; 0xFAD94 = +0x84, 0xFADDC "trade screen" = +0xCC)
//   +0x108 0xDD538 screen descriptor pointers (0x26), +0x1A0 0xDD5D0 their count (0x26)
//   +0x1A4 0x10A608 player-unit hash (128 heads; unit +0x0C id, +0xE4 next)
// Callees are reached through e.H with the registers retail sets.
#include "cl_env.h"
namespace {
enum : uint32_t {
    kState = 0x00, kDirty = 0x04, kBusy = 0x0C, kPlayer = 0x10, k3498 = 0x14, k349C = 0x18, kC434 = 0x1C,
    kPartner = 0x20, kGold = 0x28, kWaitBox = 0x2C, k77F4 = 0x30, k77FC = 0x38, kB348 = 0x3C,
    kMarkA = 0x40, kMarkB = 0x44, kBlocked = 0x58, kBC08 = 0x5C, kIUnit = 0x60, kIFlag = 0x64,
    kOpen = 0x70, kOpen94 = 0x70 + 0x14, kScreen = 0x70 + 0x5C, kDesc = 0x108, kCount = 0x1A0, kHash = 0x1A4,
};
struct X {
    const cl::Env& e; uint32_t g;
    uint32_t R(uint32_t o) const { return e.R(g + o); }
    void W(uint32_t o, uint32_t v) const { e.W(g + o, v); }
    void cancel() const { e.H(0x59650); }                                   // tail jump: the refuse/cancel sender
    void fatal(uint32_t id) const {                                         // the "bad data" exit triple (the hooked exit returns)
        const uint32_t m = e.H(0xbf92, id);
        e.H(0xbf80, 0x6FB8359Fu, m);
        e.H(0x3664, 0xffffffffu);
    }
    void mark(uint32_t ebx) const { e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, ebx); }
    bool either() const { return R(kMarkA) || R(kMarkB); }
    // the open-screen walk: per flagged screen its descriptor's +0x5C class decides; false = refused (cancel already sent)
    bool walk() const {
        uint32_t i = 0;
        do {
            if (R(kOpen + 4 * i) == 1) {
                const uint32_t cls = e.R(R(kDesc + 4 * i) + 0x5c);
                if (cls == 1) e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
                else if (cls == 4) {
                    if (R(kIFlag)) { const uint32_t edi = R(kIUnit); e.H(0x47b20); e.Edj(0x47b20, edi); }
                } else if (cls != 0) { cancel(); return false; }
            }
            ++i;
        } while ((int32_t)i < (int32_t)R(kCount));
        return true;
    }
    // after the walk: the trade screen comes up (unless already up and the player check passes); `ask` = action 0
    void show(bool ask) const {
        const uint32_t p = R(kPlayer);
        bool up;
        if (!p || (e.R(p) == 0 && e.H(0xa5440, 0, 0, 0, 0, 0, p) == 0)) { W(kScreen, 1); up = true; }
        else up = R(kScreen) != 0;
        if (up) {
            W(kC434, 0);
            if (R(k349C) == 1) {
                W(k349C, 0);
                uint32_t s = e.H(0xd0b4);
                e.H(0x3cc60, s, 0x6FBA51BCu, 0, 0, 0, 4);
                if (R(k3498) == 0) {
                    s = e.H(0xd0b4);
                    e.H(0x3ccc0, s, 0x6FBA51ECu, 0, 0, 0, 2);
                    W(k3498, 1);
                }
            }
            W(k77FC, 0);
            W(kGold, 0);
            W(kDirty, (R(kDirty) & 0xffffff00u) | 1);
            W(kB348, 0);
            e.H(0xbfa10); e.Edj(0xbfa10, 0x6FB80894u);
            const uint32_t s = e.H(0xd0b4);
            e.H(0x3ccc0, s, 0x6FB8BD18u, 0, 0, 0, 0xd);
        }
        if (ask) W(kBlocked, 0);
        W(k77F4, 0xffffffffu);
    }
    // 0x5A181: the shared tail of action 1 (also reached by action 0's and action 10's failed checks)
    void asked() const {
        if (R(kBusy)) { cancel(); return; }
        if (!walk()) return;
        show(false);
        W(kState, 2);
        e.H(0x91070, 0);
    }
};
}  // namespace

extern "C" void d2c_59ed0(uint32_t pkt, uint32_t edx, const cl::Env& e) {
    const X x{e, e.gl};
    const uint32_t act = e.R8(pkt + 1);
    switch (act) {
    case 0:
        if (x.R(kState)) { x.cancel(); return; }
        if (!x.R(kScreen)) {
            e.H(0xbf8c);
            if (x.R(kCount) != 0x26) { x.fatal(0x139b); x.asked(); return; }
            if (x.R(kBusy)) { x.cancel(); return; }
            if (!x.walk()) return;
            x.show(true);
        }
        x.W(kState, 1);
        e.H(0x599e0);
        e.H(0x91070, 0);
        return;
    case 1: {
        if (x.R(kState)) { x.cancel(); return; }
        const uint32_t p = x.R(kPlayer);
        if (e.H(0xc202, e.R(p + 0x60)) || x.R(kBC08) || !e.H(0x59ac0) || x.R(kOpen94)) {
            e.H(0x149a0, 0, 0, 0, 0, 0, 0x4f, 2, 0);
            return;
        }
        if (x.R(kScreen)) { x.W(kState, 2); e.H(0x91070, 0); return; }
        e.H(0xbf8c);
        if (x.R(kCount) != 0x26) x.fatal(0x139b);
        x.asked();
        return;
    }
    case 2:
        if (x.either()) x.mark(3);
        x.W(kMarkA, 0); x.W(kMarkB, 0);
        e.H(0x59740, 0);
        return;
    case 5: {
        if (!x.R(kScreen)) return;
        x.W(k77F4, 0xffffffffu);
        e.H(0x91070, 1);
        const uint32_t st = x.R(kState);
        if (st == 3 || st == 5) {
            if (st == 3) x.W(kState, 5);
            x.mark(x.either() ? 3 : 1);
            x.W(kMarkA, 0); x.W(kMarkB, 1);
        } else if (st == 4) {
            x.W(kState, 6);
            if (!x.R(kMarkA) || !x.R(kMarkB)) x.mark(1);
            x.W(kMarkA, 1); x.W(kMarkB, 1);
        } else if (st != 6 && st != 7) x.cancel();
        return;
    }
    case 6: {
        if (!x.R(kScreen)) return;
        const uint32_t gold = x.R(kGold);
        e.H(0x149a0, gold & 0xffff, 0, 0, 0, 0, 0x4f, 8, (edx & 0xffff0000u) | (gold >> 16));
        x.W(k77F4, 0xffffffffu);
        e.H(0x91070, 1);
        const uint32_t st = x.R(kState);
        if (st == 1) {
            x.W(k77FC, 0);
            if (x.either()) x.mark(3);
            x.W(kMarkA, 0); x.W(kMarkB, 0);
            x.W(kState, 3);
            if (x.R(kWaitBox)) { e.H(0xa3d20); x.W(kWaitBox, 0); }
        } else if (st >= 3 && st <= 7) {
            if (st != 3) x.W(kState, 3);
            if (x.either()) x.mark(3);
            x.W(kMarkA, 0); x.W(kMarkB, 0);
        } else x.cancel();
        return;
    }
    case 9:
        e.H(0x23200, x.R(kPlayer), 0x17);
        return;
    case 10: {
        const uint32_t id = x.R(kPartner);
        uint32_t u = x.R(kHash + 4 * (id & 0x7f));
        if (!u) return;
        while (e.R(u + 0xc) != id) { u = e.R(u + 0xe4); if (!u) return; }
        if (e.R(u) != 0) { x.fatal(0x5a); x.asked(); return; }
        e.H(0x23200, u, 0x17);
        return;
    }
    case 0xc: x.W(kState, 0); e.H(0x59740, 0); return;
    case 0xd: x.W(kState, 0); e.H(0x59740, 1); return;
    case 0xe: x.W(kBlocked, 1); return;
    case 0xf: x.W(kBlocked, 0); return;
    case 0x10: e.H(0x984a0); return;                                     // tail jumps (stash / cube screens, other rows)
    case 0x11: e.H(0x98320); return;
    case 0x15: e.H(0x98070); return;
    default: return;
    }
}
