// D2Common 1.13c tile-record allocator for the generic kinds (recursive for kind 3). Registers (ecx room, edx head, ebx flags,
// esi record) are explicit; the flag pass 0x68A90, the roll 0x68B90 and the D2CMP handle-flags import (thunk 0x92FA) are seams.
// Struct words are 32-bit retail offsets, addresses fit 32 bits.
#include <cstdint>
#include <cstring>

struct D2TileRec68F00Seam {
    // 0x68A90: ECX room, EAX y, EBX flags, ESI record, stack v7 then x
    void (*pfnFlagPass)(uint32_t room, int32_t y, uint32_t flags, uint32_t rec, uint32_t v7, int32_t x);
    // 0x68B90: EAX flags, ECX room, stack 4
    uint32_t (*pfnRoll)(uint32_t flags, uint32_t room, uint32_t four);
    // thunk 0x92FA
    uint32_t (*pfnHandleFlags)(uint32_t);
};

namespace {
inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a) + o), 4); return v; }
inline void wr(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(reinterpret_cast<void*>(uintptr_t(a) + o), &v, 4); }
}

// 1.13c D2Common+0x68F00
extern "C" uint32_t D2Common_68F00_AllocTileRec(const D2TileRec68F00Seam* s, uint32_t room, uint32_t head, int32_t x, int32_t y,
                                                uint32_t flags, uint32_t v6, uint32_t v7)
{
    const uint32_t pool = rd(room, 0x54);
    const uint32_t rec = rd(pool, 0x14) + rd(pool, 8) * 0x30;
    if (head == 0) wr(rec, 0x20, 0);
    else { wr(rec, 0x20, rd(head, 0)); wr(head, 0, rec); }
    wr(pool, 8, rd(pool, 8) + 1);
    wr(rec, 8, uint32_t(x) - rd(room, 0x34));
    wr(rec, 0xc, uint32_t(y) - rd(room, 0x38));
    wr(rec, 0, uint32_t((x - (y + 1)) * 0x50));
    wr(rec, 4, uint32_t(((y + 1 + x) * 0x50) >> 1) + 0x28);
    wr(rec, 0x18, v6);
    wr(rec, 0x1c, v7);
    wr(rec, 0x14, 0);
    wr(rec, 0x24, 0);
    uint8_t* b = reinterpret_cast<uint8_t*>(uintptr_t(rec));
    b[0x29] = 0xff; b[0x2a] = 0xff; b[0x28] = 0xff;
    s->pfnFlagPass(room, y, flags, rec, v7, x);
    if (v7 == 3) {
        const uint32_t roll = s->pfnRoll(flags, room, 4);
        const uint32_t c = D2Common_68F00_AllocTileRec(s, room, head, x, y, flags, roll, 4);
        uint32_t f = rd(c, 0x14) | ((((flags >> 0x12) & 3) << 0xe) + 0x4000);
        if (int8_t(flags) < 0) f |= 1;
        if (flags & 0x10000000) f |= 0x102;
        if (flags & 0x20000) f |= 0x40;
        if (flags & 0x10000) f |= 0x80;
        if (flags & 8) f |= 4;
        f = (int32_t(flags) < 0) ? (f | 8) : (f & 0xfffffff7u);
        if (flags & 0x4000000) f |= 0x20c;
        if (flags & 0x20000000) f |= 0x800;
        if (flags & 4) f |= 0x2000;
        const uint32_t u = s->pfnHandleFlags(rd(c, 0x18));
        if (u & 1) f |= 4;
        if (u & 4) f |= 0x800;
        wr(c, 0x14, f);
    }
    return rec;
}
