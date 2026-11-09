// src/core/d2game/d2game_d62e0.cpp -- 1.13c D2Game+0xD62E0 MoveItemInventoryToCursor, written from the 1.13c disassembly
// (the Ghidra decompile drops ECX/EAX and the stack-argument count, and misreads the flag words as callee results).
// Convention as d2game_71330.cpp: retail register inputs explicit and first (ECX, EAX), then the stack words; callees are weak externs.
// The rule the capture proves is in d2game_d62e0.h.
#include <cstdint>

typedef intptr_t I;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char D2GAME_DAT_F9FF9[];   // the fatal-error source-file string (retail .rdata 0x6FD19FF9)
extern "C" __attribute__((weak)) char D2GAME_DAT_FB14C[];   // "ItemMode.cpp" (retail .rdata 0x6FD1B14C)
O(11017); O(10020); O(10646); O(11057); O(10201); O(10584); O(10869); O(11030); O(10012); O(10466); O(11090);
O(10977); O(10892); O(10202); O(10050); O(10265); O(10024);
F(37680); F(D29F0); F(D46B0); F(D51A0); F(D4630); F(1130); F(1100); F(71330); F(D1BC0); F(2A3C);

static void fatal(U line) { Ord10024(D2GAME_DAT_F9FF9, Ord10265(line)); D2GAME_2A3C(0xffffffff); }

// 1.13c D2Game+0xD62E0 -- ECX = self unit, EAX = inventory override (0: self+0x60), stack (ret 0x18) = game, item id, out word*,
// a3 (act the client-side tail), a4 (flag handed to #10977), a5 (skip the #11017 lock gate). Returns 1 (moved) or 0.
// Retail stores the item kind in the low byte of the out-pointer argument slot and hands the whole slot to #10584, #11030 and 0x1130.
extern "C" U D2GAME_D62E0(I self, I eax, I game, U id, I out, U a3, U a4, U a5)
{
    if (!game) fatal(0x907);
    if (!self) fatal(0x908);
    R32(out, 0) = 0;
    I inv = (I)R32(self, 0x60);
    if (eax) inv = eax;
    if (!inv) fatal(0x911);
    if (Ord11017(inv) && !a5) return 0;
    I item = (I)R32(game, 0x1720 + (id & 0x7F) * 4);
    while (item && R32(item, 0xC) != id) item = (I)R32(item, 0xE4);
    if (!item) { R32(out, 0) = 1; return 0; }
    if (R32(item, 0) != 4) return 0;
    if (R32(self, 0) == 0 && !D2GAME_37680(self) && (Ord10020(item) & 0xFF)) { R32(out, 0) = 1; return 0; }
    if (R32(item, 0x10)) { R32(out, 0) = 1; return 0; }
    D2GAME_D29F0(self);
    R32(item, 0xC4) &= ~2u;
    I p = Ord10646(inv, item);
    if (!p) fatal(0x948);
    if (p != item) fatal(0x949);
    D2GAME_D46B0(self /*ESI*/, item /*EDI*/, game, 0, 1);
    if (Ord11057(item, self)) { D2GAME_D51A0(game, self, 0); Ord10201(self, 1); }
    D2GAME_D4630(item /*EAX*/, self /*ECX*/);
    const U kind = Ord10020(item) & 0xFF;
    const U slot = ((U)out & ~0xFFu) | kind;
    const I r = Ord10584(self, slot, R32(game, 0x70));
    U ebx = 1, a5p = 0;
    if (R32(self, 0) != 0 || kind == 1) ebx = 0; else if (kind == 2) a5p = 1;
    const I A = D2GAME_1130(item /*ECX*/, r, 0, slot);
    const I B = D2GAME_1100(item /*ECX*/, A);
    Ord10869(D2GAME_DAT_FB14C, 0x966, inv, B, A, r, 0, slot);   // the trace call: the 8 words retail leaves on the stack for its ret 0x20
    Ord11030(item, slot);
    Ord10012(item, 0xFF);
    if (ebx) Ord10466(inv, item);
    Ord11090(item, 4);
    if (a3 != 1) return 1;
    if (a4) Ord10977(item, 0x20, 1);
    if (a5p == 1) D2GAME_71330(game /*ECX*/, self /*EDX*/, item);
    Ord10892(item, 4, 1);
    if (D2GAME_D1BC0(item /*EAX*/)) Ord10977(item, 1, 1);
    if (Ord10202(item, 0x4000, 0x979, D2GAME_DAT_FB14C)) Ord10977(item, 0x4000, 0);
    Ord10050(inv, item);
    Ord10201(self, 1);
    return 1;
}
