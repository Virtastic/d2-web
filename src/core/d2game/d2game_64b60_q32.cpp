// 0x64B60 (EAX = unit, stack = quest record, ret 4): A1Q3 "player or party has the malus" test. A null unit is fatal (0xA2DA 0xA2A, 0xA2C2, 0x2A3C -1; not driven).
// 0xE660 (EAX unit, 0x206D6468) true: return 1. Else unit + 0x80 (EDI) must be set and unit type 0, else 0. 0x9BEB0 (EAX unit, EDI) returning 0xFFFF: 0.
// Otherwise data (quest + 0x18) + 0xA0 := 0 and 0x9C160 (EAX the 0x9BEB0 result, ESI game = quest + 4, EBX 0, EDI, stack callback 0x61B20) walks the party;
// the callback sets data + 0xA0. Return data + 0xA0 != 0.
#include "d2game_q32.h"
using namespace q32;
static uint32_t M(uint32_t rva) { return 0xB2000000u | rva; }
// 1.13c D2Game+0x64B60. EDI is carried to the recorder in the c slot for 0x9BEB0 and 0x9C160.
bool d2game_64b60_q32(const Call& X, uint32_t unit, uint32_t q)
{
    if (!unit) {
        const uint32_t v = X(0xA2DA, 0, 0, 0, 0, 0, 0xa2a);
        X(0xA2C2, 0, 0, 0, 0, 0, M(0xF9FF9), v);
        X(0x2A3C, 0, 0, 0, 0, 0, 0xffffffffu);
        return false;
    }
    if (X(0xE660, unit, 0, 0, 0, 0, 0x206d6468)) return true;
    const uint32_t data = rd32(q, 0x18), edi = rd32(unit, 0x80);
    if (!edi || rd32(unit, 0) != 0) return false;
    const uint32_t r = X(0x9BEB0, unit, edi);
    if ((r & 0xffff) == 0xffff) return false;
    wr8(data, 0xa0, 0);
    X(0x9C160, r, edi, 0, 0, rd32(q, 4), M(0x61b20));
    return rd8(data, 0xa0) != 0;
}
