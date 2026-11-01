// 1.13c D2Common+0x42070
// EDI = address of the list head pointer; the list is circular, nodes are 0x14 bytes:
// +0 x, +4 y (int32), +8 type byte, +0xC flags (dword), +0x10 next.
// Pass 1 rounds x and y to the 8-unit grid: (v + sign fix) >> 3. Pass 2 folds a node's successor into the
// node when both grid cells match: flags OR, type byte copied, successor unlinked; a removed head makes the node the head.
// The debug log call (file and line 0x2CB) is not reproduced.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t Rd32(uintptr_t p) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(p), 4); return v; }
inline void Wr32(uintptr_t p, uint32_t v) { std::memcpy(reinterpret_cast<void*>(p), &v, 4); }
inline int32_t Grid(int32_t v) {
    uint32_t add = static_cast<uint32_t>(v >> 31) & 7u;
    return static_cast<int32_t>(static_cast<uint32_t>(v) + add) >> 3;
}
}

extern "C" void D2Common_42070(uint32_t pHead) {
    uintptr_t n = Rd32(pHead);
    do {
        Wr32(n + 0, static_cast<uint32_t>(Grid(static_cast<int32_t>(Rd32(n + 0)))));
        Wr32(n + 4, static_cast<uint32_t>(Grid(static_cast<int32_t>(Rd32(n + 4)))));
        n = Rd32(n + 0x10);
    } while (n != Rd32(pHead));

    n = Rd32(pHead);
    do {
        uintptr_t m = Rd32(n + 0x10);
        if (Rd32(n) == Rd32(m) && Rd32(n + 4) == Rd32(m + 4)) {
            if (m == Rd32(pHead)) Wr32(pHead, static_cast<uint32_t>(n));
            Wr32(n + 0x10, Rd32(m + 0x10));
            Wr32(n + 0xC, Rd32(n + 0xC) | Rd32(m + 0xC));
            *reinterpret_cast<uint8_t*>(n + 8) = *reinterpret_cast<uint8_t*>(m + 8);
        }
        n = Rd32(n + 0x10);
    } while (n != Rd32(pHead));
}
