// 1.13c D2Game+0xB8A70 -- ECX = quest record. A +0xC byte other than 5 with a non-zero +9 byte returns the record pointer with AL = 1. Otherwise the helper 0x33F30 (EAX = the record's +4 game, ECX = its +0x10 word) gives a record; none returns 0, else the result of its +0xF0 callback (ECX = that record) is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb8a70(GE& e, uint32_t q)
{
    if (e.rd8(q + 0xc) != 5 && e.rd8(q + 9)) return (q & ~0xffu) | 1;
    const uint32_t n = e.h(0x33F30, e.rd32(q + 4), e.rd32(q + 0x10), 0, 0, 0);
    if (!n) return 0;
    return e.h(e.rd32(n + 0xf0) & 0xffffffu, 0, n, 0, 0, 0);   // the callback, a code address
}
}
