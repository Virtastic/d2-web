// D2Common 0x2E1F0: allocate the active room for a DRLG room. ESI (the DRLG room) is the explicit second parameter.
#include <cstdint>
#include <cstring>
#include "D2Dungeon.h"

template <class T> static inline T& F(void* p, int off) { return *(T*)((uint8_t*)p + off); }
// retail pointer fields are 32-bit
#define PT(p, o) ((void*)(uintptr_t)F<uint32_t>(p, o))

// 1.13c D2Common+0x2E1F0 (stdcall one arg = the DRLG object; hidden ESI = the room). Builds the tile rect (x5, y5, w5, h5 then the
// raw x, y, w, h from +0x34..+0x40), and for a room whose tile record has data at +0x18 or +0x20 steps the room's own seed
// (seed = seed_lo * 0x6AC690C5 + seed_hi, 64-bit, back into +0x14/+0x18) and allocates the active room from it.
// Flags: 4 if room flags have 0x40000, else 1 if byte +0x60 has bit 0, else 0.
void sub_2E1F0(void* pDrlg, void* pRoom) {
    int32_t c[8];
    c[4] = F<int32_t>(pRoom, 0x34); c[5] = F<int32_t>(pRoom, 0x38);
    c[6] = F<int32_t>(pRoom, 0x3c); c[7] = F<int32_t>(pRoom, 0x40);
    for (int i = 0; i < 4; ++i) c[i] = c[4 + i] * 5;
    uint8_t* tiles = (uint8_t*)PT(pRoom, 0x54) + 0x14;
    if (F<uint32_t>(tiles, 4) == 0 && F<uint32_t>(tiles, 0xc) == 0) return;
    uint32_t flags = 0;
    if (F<uint32_t>(pRoom, 0x28) & 0x40000) flags = 4;
    else if (F<uint8_t>(pRoom, 0x60) & 1) flags = 1;
    uint64_t s = (uint64_t)F<uint32_t>(pRoom, 0x14) * 0x6ac690c5u + F<uint32_t>(pRoom, 0x18);
    F<uint32_t>(pRoom, 0x14) = (uint32_t)s;
    F<uint32_t>(pRoom, 0x18) = (uint32_t)(s >> 32);
    void* act = PT(pDrlg, 0x46c);
    void* r = DUNGEON_AllocRoom((D2DrlgActStrc*)act, (D2DrlgRoomStrc*)pRoom, (D2DrlgCoordsStrc*)c,
                                (D2DrlgRoomTilesStrc*)tiles, (int)(uint32_t)s, flags);
    F<uint32_t>(pRoom, 0x30) = (uint32_t)(uintptr_t)r;
}
