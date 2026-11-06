// D2Common 1.13c tile-record linker. EDI (room) is an explicit parameter; the id table 0x19950, the tile gate 0x69090, the node
// touch 0x68800, the roll 0x68B90 and the record allocator 0x68F00 are seams. The two fatal paths of retail (no node for the id,
// node without a definition) call pfnFatal and return. Struct words are 32-bit retail offsets, addresses fit 32 bits.
#include <cstdint>
#include <cstring>

struct D2TileLink69360Seam {
    uint32_t (*pfnGetTable)(uint32_t drlg, uint32_t id);                                   // 0x19950 (EAX, EDX)
    uint32_t (*pfnGate)(uint32_t flags, uint32_t room, int32_t x, int32_t y, uint32_t type); // 0x69090 (EAX, ECX, stack 3)
    void (*pfnTouch)(uint32_t node, uint32_t type);                                          // 0x68800 (ESI node, stack 1)
    uint32_t (*pfnRoll)(uint32_t flags, uint32_t room, uint32_t type);                       // 0x68B90 (EAX, ECX, stack 1)
    uint32_t (*pfnAlloc)(uint32_t room, uint32_t head, int32_t x, int32_t y, uint32_t flags, uint32_t v6, uint32_t v7); // 0x68F00
    void (*pfnFatal)(uint32_t id);                                                           // 0x2b1 / 0x2cb: message and _exit(-1)
};

namespace {
inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a) + o), 4); return v; }
inline void wr(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(reinterpret_cast<void*>(uintptr_t(a) + o), &v, 4); }
}

// 1.13c D2Common+0x69360
extern "C" void D2Common_69360_LinkTileRec(const D2TileLink69360Seam* s, uint32_t room, uint32_t rec, uint32_t flags, uint32_t type)
{
    const uint32_t lvl = rd(room, 0x58);
    const uint32_t tbl = s->pfnGetTable(rd(lvl, 0x1b4), rd(lvl, 0x1d0));
    const uint32_t kind = flags >> 8;
    uint32_t node = rd(room, 0x4c);
    const uint32_t key = rd(tbl, ((flags >> 0x14) & 0x3f) * 4);
    for (;;) {
        if (node == 0) { s->pfnFatal(0x2b1); return; }
        if (rd(rd(node, 0x10), 0) == key) break;
        node = rd(node, 4);
    }
    const int32_t sx = int32_t(rd(rec, 8) + rd(room, 0x34));
    const int32_t sy = int32_t(rd(room, 0x38) + rd(rec, 0xc));
    if ((uint8_t(kind) == 0 || uint8_t(kind) == 4) && s->pfnGate(flags, room, sx, sy, type) == 0) return;
    s->pfnTouch(node, type);
    wr(rec, 0x20, rd(node, 0xc));
    const uint32_t def = rd(node, 0x10);
    wr(node, 0xc, rec);
    if (def == 0) { s->pfnFatal(0x2cb); return; }
    if (rd(def, 0x24) == 0) return;
    const uint32_t f2 = (rd(def, 0x28) << 8) | flags;
    const uint32_t roll = s->pfnRoll(f2, room, type);
    const uint32_t r2 = s->pfnAlloc(room, node + 0x14, sx, sy, f2, roll, type);
    if (r2) wr(r2, 0x14, rd(r2, 0x14) | 8);
}
