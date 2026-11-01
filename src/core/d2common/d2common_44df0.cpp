// 1.13c D2Common+0x44DF0
// Builds a room's neighbour list: EDI = room, one stack argument (the memory pool). Every room of the level (level at room +0x58, first room at
// level +0x10, chain through +0x24) whose box is within 5 tiles of this room on both axes (gap = distance between the boxes, x at +0x34/width
// +0x3C, y at +0x38/height +0x40) is collected in chain order and its count stored at room +0x2C; the collected array is sorted by 0x445A0
// (EAX = count, ESI = array); a pool block of count*4 bytes is allocated (file DrlgRoom.cpp, line 0x2CB), stored at room +8 and filled with the
// sorted array. Links are 32-bit retail tokens: `resolve` turns one into an address; `sort` sorts the token array in place; `alloc` returns a token.
#include <cstdint>
#include <cstring>

extern "C" uint32_t D2Common_44DF0(uint8_t* pRoom, uint32_t pool, uint8_t* (*resolve)(uint32_t), void (*sort)(uint32_t* pArray, int32_t nCount),
                               uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line))
{
    auto rd = [](const uint8_t* b, uint32_t o) { return static_cast<int32_t>([&] { uint32_t v; std::memcpy(&v, b + o, 4); return v; }()); };
    auto wr = [](uint8_t* b, uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); };
    uint32_t found[64];
    wr(pRoom, 0x2C, 0);
    int32_t n = 0;
    for (uint32_t tok = static_cast<uint32_t>(rd(resolve(static_cast<uint32_t>(rd(pRoom, 0x58))), 0x10)); tok;) {
        const uint8_t* o = resolve(tok);
        int32_t dx = rd(pRoom, 0x34) < rd(o, 0x34) ? rd(o, 0x34) - rd(pRoom, 0x3C) - rd(pRoom, 0x34) : rd(pRoom, 0x34) - rd(o, 0x3C) - rd(o, 0x34);
        int32_t dy = rd(pRoom, 0x38) < rd(o, 0x38) ? rd(o, 0x38) - rd(pRoom, 0x40) - rd(pRoom, 0x38) : rd(pRoom, 0x38) - rd(o, 0x40) - rd(o, 0x38);
        if (dx < 6 && dy < 6) { found[n++] = tok; wr(pRoom, 0x2C, static_cast<uint32_t>(n)); }
        tok = static_cast<uint32_t>(rd(o, 0x24));
    }
    sort(found, rd(pRoom, 0x2C));
    wr(pRoom, 8, alloc(pool, static_cast<uint32_t>(n) * 4, 0x2CB));
    for (int32_t i = 0; i < rd(pRoom, 0x2C); ++i) wr(resolve(static_cast<uint32_t>(rd(pRoom, 8))), static_cast<uint32_t>(i) * 4, found[i]);
    return static_cast<uint32_t>(n);   // EAX at the return: the copy loop counter, ending at the count (0 for none)
}
