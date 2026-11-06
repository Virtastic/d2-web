// D2Common 1.13c tile-record allocator for kind 0xd. Registers (ecx room, edx x, ebx flags) are explicit parameters; the
// D2CMP handle-flags import (thunk 0x92FA) is a seam. Struct words are 32-bit retail offsets, addresses fit 32 bits.
#include <cstdint>
#include <cstring>

struct D2TileRecSeam { uint32_t (*pfnHandleFlags)(uint32_t); };

namespace {
inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a) + o), 4); return v; }
inline void wr(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(reinterpret_cast<void*>(uintptr_t(a) + o), &v, 4); }
}

// 1.13c D2Common+0x68CD0: take record (pool+0x24)[pool+0x10 * 0x30] from the pool at room+0x54, link it on *head (+0x20), fill the
// room-relative tile and screen words, kind 0xd, then build the flag word at +0x14 from the packed flags and the handle's flag import.
extern "C" uint32_t D2Common_68CD0_AllocTileRec(const D2TileRecSeam* s, uint32_t room, int32_t x, uint32_t flags,
                                                uint32_t head, int32_t y, uint32_t handle)
{
    const uint32_t pool = rd(room, 0x54);
    const uint32_t rec = rd(pool, 0x24) + rd(pool, 0x10) * 0x30;
    if (head == 0) wr(rec, 0x20, 0);
    else { wr(rec, 0x20, rd(head, 0)); wr(head, 0, rec); }
    wr(pool, 0x10, rd(pool, 0x10) + 1);
    wr(rec, 8, uint32_t(x) - rd(room, 0x34));
    wr(rec, 0xc, uint32_t(y) - rd(room, 0x38));
    wr(rec, 0, uint32_t((x - (y + 1)) * 0x50));
    wr(rec, 4, uint32_t(((y + 1 + x) * 0x50) >> 1) + 0x28);
    wr(rec, 0x18, handle);
    wr(rec, 0x1c, 0xd);
    wr(rec, 0x14, 0);
    wr(rec, 0x24, 0);
    uint8_t* b = reinterpret_cast<uint8_t*>(uintptr_t(rec));
    b[0x29] = 0xff; b[0x2a] = 0xff; b[0x28] = 0xff;
    uint32_t f = 0;
    if (int8_t(flags) < 0) f |= 1;
    if (flags & 0x10000000) f |= 0x102;
    if (flags & 0x20000) f |= 0x40;
    if (flags & 0x10000) f |= 0x80;
    if (flags & 8) f |= 4;
    f = (int32_t(flags) < 0) ? (f | 8) : (f & 0xfffffff7u);
    if (flags & 0x4000000) f |= 0x20c;
    if (flags & 0x20000000) f |= 0x800;
    if (flags & 4) f |= 0x2000;
    const uint32_t u = s->pfnHandleFlags(handle);
    if (u & 1) f |= 4;
    if (u & 4) f |= 0x800;
    wr(rec, 0x14, f);
    return rec;
}
