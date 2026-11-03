// The player-save stat block reader on retail's 32-bit layout (the vendored typed form is PLRSAVE2_ReadStats in
// d2moo/src/D2Game/PLAYER/PlrSave2.cpp). EAX = save version, ECX = end of the save buffer; four stack words (an unused
// slot, the unit, the cursor slot, the stat count in bits), ret 0x10. A version past 0x5E hands (end, unit, cursor) to
// the newer bit-packed reader. Otherwise the block is the word 0x6667, a presence bitmask of (bits+7)/8 bytes and one
// dword per set bit; either running past the end answers 0x12. Every stat id below the count is written (unit, id,
// value or 0, layer 0); the cursor ends past the last value read and the answer is 0.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2GEBDA0Env {
    uint32_t bitMasks;                                                      // the D2Common bitmask table (1 << n per dword)
    uint32_t (*readNew)(uint32_t end, uint32_t unit, uint32_t cursor);     // the newer reader (EAX end, two stack words)
    uint32_t (*countBits)(uint32_t mask, uint32_t bytes);                  // set bits in a byte run
    void     (*setStat)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer);
};

uint32_t PLRSAVE2_ReadStats32(uint32_t version, uint32_t end, uint32_t, uint32_t unit, uint32_t cursor, int32_t bits,
                              const D2GEBDA0Env& env)
{
    if (version > 0x5E) return env.readNew(end, unit, cursor);
    const int32_t bytes = (bits + 7) / 8;
    uint32_t p = rd32(cursor);
    if (p + bytes + 2 > end || rd16(p) != 0x6667) return 0x12;
    const uint32_t mask = p + 2, vals = mask + bytes;
    if (vals + env.countBits(mask, bytes) * 4 > end) return 0x12;
    uint32_t k = 0;
    for (int32_t i = 0; i < bits; i++) {
        uint32_t v = 0;
        if (rd8(mask + (i >> 3)) & rd32(env.bitMasks + (i & 7) * 4)) v = rd32(vals + 4 * k++);
        env.setStat(unit, (uint32_t)i, v, 0);
    }
    wr32(cursor, vals + 4 * k);
    return 0;
}
