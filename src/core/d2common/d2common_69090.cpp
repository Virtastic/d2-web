// D2Common 1.13c tile-event gate. Registers (eax packed flags, ecx room) are explicit parameters; the table lookup 0x19950, the
// record finder 0x6C5D0 and the event allocator 0x44480 are seams. Struct words are 32-bit retail offsets, addresses fit 32 bits.
#include <cstdint>
#include <cstring>

struct D2TileEvt69090Seam {
    // 0x19950: EAX = (level+0x58)->+0x1b4, EDX = (level+0x58)->+0x1d0; returns the 64-entry id table
    uint32_t (*pfnGetTable)(uint32_t drlg, uint32_t id);
    // 0x6C5D0: EBX = table entry, DL = 0x6c or 0x72; returns the record or 0
    uint32_t (*pfnFindRec)(uint32_t entry, uint32_t kind);
    // 0x44480: ESI = room, stack 5, rec word 0, 0, x', y'
    void (*pfnQueue)(uint32_t room, uint32_t five, uint32_t id, uint32_t zero, uint32_t sx, uint32_t sy);
};

namespace {
inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a) + o), 4); return v; }
}

// 1.13c D2Common+0x69090
extern "C" uint32_t D2Common_69090_TileEvent(const D2TileEvt69090Seam* s, uint32_t flags, uint32_t room, int32_t x, int32_t y,
                                             uint32_t type)
{
    const uint32_t lvl = rd(room, 0x58);
    const uint32_t tbl = s->pfnGetTable(rd(lvl, 0x1b4), rd(lvl, 0x1d0));
    const uint32_t entry = rd(tbl, ((flags >> 0x14) & 0x3f) * 4);
    const uint32_t kind = uint8_t(uint8_t(type != 0xb ? 0 : 0xff) & 6) + 0x6c;
    const uint32_t rec = s->pfnFindRec(entry, kind);
    if (rec == 0) return 0;
    const int32_t xr = x - int32_t(rd(room, 0x34));
    const int32_t yr = y - int32_t(rd(room, 0x38));
    if (xr == int32_t(rd(room, 0x3c)) || yr == int32_t(rd(room, 0x40))) return 0;
    s->pfnQueue(room, 5, rd(rec, 0), 0, uint32_t(xr * 5) + rd(rec, 0x1c), uint32_t(yr * 5) + rd(rec, 0x20));
    return 1;
}
