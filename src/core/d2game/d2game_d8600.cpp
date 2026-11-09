// src/core/d2game/d2game_d8600.cpp -- 1.13c D2Game+0xD8600 move an item into a belt/body slot, the occupant out, written from the 1.13c disassembly (the cached decompile
// and the earlier batch port reorder the callees and misread the slot byte). Convention as d2game_d62e0.cpp: retail register inputs explicit
// and first (ECX = self, EAX = item id; ESI/EDI/EBX of the register-convention callees before their stack words), callees are weak externs.
// The rule the capture proves is in d2game_d8600.h.
#include <cstdint>

typedef intptr_t I;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char D2GAME_DAT_F9FF9[];   // the fatal-error source-file string (retail .rdata 0x6FD19FF9)
extern "C" __attribute__((weak)) char D2GAME_DAT_FB14C[];   // "ItemMode.cpp" (retail .rdata 0x6FD1B14C)
O(10265); O(10024); O(11139); O(10605); O(10946); O(10244); O(10061); O(11055); O(10013); O(10694); O(10646); O(10608); O(11088);
O(10466); O(11090); O(11041); O(10405); O(10058); O(10138); O(10012); O(10892); O(10977); O(10202); O(10050); O(10201);
F(2A3C); F(D29F0); F(D46B0); F(D47C0); F(DFFD0); F(D7E30); F(D28D0); F(D51A0);

static void fatal(U line) { Ord10024(D2GAME_DAT_F9FF9, Ord10265(line)); D2GAME_2A3C(0xffffffff); }

// 1.13c D2Game+0xD8600 -- ECX = self unit, EAX = item id, stack (ret 0x10) = game, a2 (slot byte, >= 11 returns 0 first), a3 (flag), out word*.
// AA78 (#10946) must return 5 and the occupant must have mode 1. Returns 1 (moved) or 0.
extern "C" U D2GAME_D8600(I self, U id, I game, U a2, U a3, U* out)
{
    if (!game) fatal(0xca8);
    if (!self) fatal(0xca9);
    if ((a2 & 0xFF) >= 0xB) return 0;
    *out = 0;
    D2GAME_D29F0(self);
    I it = (I)R32(game, 0x1720 + (id & 0x7F) * 4);
    while (it && R32(it, 0xC) != id) it = (I)R32(it, 0xE4);
    if (!it) { *out = 0; return 0; }
    if (R32(it, 0) != 4) return 0;
    if (R32(it, 0x10) != 4) { *out = 0; return 0; }
    const U slot = a2 & 0xFF;
    if (Ord10946(self, slot, it, a3) != 5) { *out = 0; return 0; }
    const I inv = (I)R32(self, 0x60);
    const I t = Ord10061(inv);
    if (t) Ord11055(self, t);
    I other = Ord11139(inv, slot);
    Ord10605(self, inv, &other, slot);
    if (!other || R32(other, 0x10) != 1) { *out = 1; return 0; }
    D2GAME_D46B0(self /*ESI*/, other /*EDI*/, game, 0, 1);
    if (a3 == 0 && !Ord10244(it, self, 0, 0, 0, 0)) {
        D2GAME_D47C0(self /*ESI*/, other /*EDI*/, inv /*EBX*/, 0);
        D2GAME_DFFD0(self /*ESI*/, 0x14, 0);
        return 0;
    }
    if (Ord11088(it) == 0x13) D2GAME_D7E30(it /*EAX*/, game, self);
    Ord10013(self, other);
    Ord10694(inv, other);
    const I p = Ord10646(inv, other);
    if (!p) fatal(0xcef);
    if (other != p) fatal(0xcf0);
    Ord10608(inv, slot);
    Ord10466(inv, other);
    if (other) R32(other, 0xC4) &= ~2u;
    Ord11090(other, 4);
    Ord10977(other, 0x80, 1);
    Ord10892(other, 0x20, 1);
    Ord10977(other, 1, 1);
    if (Ord10202(other, 0x4000, 0xcf8, D2GAME_DAT_FB14C)) Ord10977(other, 0x4000, 0);
    Ord10050(inv, other);
    if (!Ord11041(inv, it, slot)) { *out = 1; return 0; }
    if (!Ord10405(inv, it, 3)) { *out = 1; return 0; }
    Ord10058(it, a2);
    Ord10138(inv, it);
    D2GAME_D47C0(self /*ESI*/, it /*EDI*/, inv /*EBX*/, 0);
    R32(it, 0xC4) &= ~2u;
    Ord11090(it, 1);
    Ord10012(it, 0xFF);
    Ord10977(it, 0x40, 1);
    Ord10977(it, 1, 1);
    Ord10892(it, 0x20, 1);
    Ord10977(it, 1, 1);
    if (Ord10202(it, 0x4000, 0xd13, D2GAME_DAT_FB14C)) Ord10977(it, 0x4000, 0);
    Ord10050(inv, it);
    D2GAME_D28D0(self /*EAX*/);
    D2GAME_D51A0(game, self, 0);
    return 1;
}
