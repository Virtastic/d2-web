// 1.13c D2Game+0xA3E10: same-level and distance gate between two units.
// fastcall ecx = unit1, edx = unit2. Callees are seams in retail argument order; B08F0 takes unit2 in EAX
// (the 0xA3E80-era decompile lost it) and the two position words on the stack, x first.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include "d2game_a3e10.h"

typedef uintptr_t A;
static inline uint8_t rd8(A p, uint32_t o) { return ((const uint8_t*)p)[o]; }

// 1.13c D2Game+0xA3E10
extern "C" EMSCRIPTEN_KEEPALIVE
bool d2_game_A3E10(A unit1, A unit2, const D2GameA3E10Env* e)
{
    if (!unit1) { e->o10024(e->assertFile, e->o10265(0x36C)); std::exit(-1); }
    if (!unit2) { e->o10024(e->assertFile, e->o10265(0x36D)); std::exit(-1); }
    if (rd8(unit1, 0x18) != rd8(unit2, 0x18)) return false;
    A y = e->f21130(unit1);
    A x = e->f21100(unit1);
    return (uint32_t)e->fB08F0(unit2, x, y) < 0x3FC;
}
