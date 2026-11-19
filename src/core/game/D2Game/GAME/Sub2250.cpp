#include "GAME/Game113c.h"

#include <cstdint>
#include <cstring>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

// A link word is either a direct pointer (high bit clear) or a complement of one (high bit set); a direct link is an offset from the pair
// address plus the head's +0x04 base -- the same encoding the 1.10f D2HashTableStrc/D2HashItemStrc pair uses. The unlink writes the head
// into the resolved slot, writes the link into head+0x04, and clears both words of the pair.
static void UnlinkPair(void* entry, uint32_t pairOff)
{
    const int32_t head = (int32_t)(intptr_t)rdptr(entry, pairOff);
    if (head == 0) return;
    const uint32_t link = rd32(entry, pairOff + 4);
    uint8_t* slot;
    if ((int32_t)link < 0) {
        slot = (uint8_t*)(uintptr_t)(uint32_t)~link;
    } else {
        slot = (uint8_t*)entry + pairOff + (link - rd32((const void*)(intptr_t)head, 4));
    }
    *(uint32_t*)(void*)slot = (uint32_t)head;
    wr32((void*)(intptr_t)head, 4, link);
    wr32(entry, pairOff, 0);
    wr32(entry, pairOff + 4, 0);
}

// 1.13c D2Game+0x2250
// Take EAX's record out of two intrusive chains: the pair at +0x04/+0x08, then the pair at +0x0C/+0x10. The second pair is examined only when
// the first pair's second word is nonzero. This is the 32-bit-layout TSHashObject unlink; the wasm64 game's TSHashTable keeps native links,
// so the game body is reached through the probe only.
void sub_6FC22250(uint32_t* pEntry)
{
    void* entry = pEntry;
    if (rd32(entry, 8) == 0) return;
    UnlinkPair(entry, 4);
    UnlinkPair(entry, 0x0C);
}
