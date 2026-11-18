// 1.13c D2Common+0x2D420: flag the rooms of a DRLG level chain (fastcall: EDX = DRLG, ECX unused; void). When the DRLG's
// counter (+0x8) and its counter array (+0x22C) are both set, each level on the chain (+0x10, next +0x24) gets bit 0 of
// its +0x60 word when the counter at its index is nonzero.
#include <cstdint>

extern "C" void D2Common_2D420_MarkRoomChainFlag(uint32_t unusedEcx, uint32_t drlg)
{
    (void)unusedEcx;
    auto W = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
    if (!W(drlg + 0x8) || !W(drlg + 0x22C)) return;
    uint32_t node = W(drlg + 0x10);
    if (!node) return;
    for (uint32_t i = 0;; i += 4) {
        if (W(W(drlg + 0x22C) + i)) W(node + 0x60) |= 1u;
        node = W(node + 0x24);
        if (!node) break;
    }
}
