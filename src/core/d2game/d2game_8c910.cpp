// 1.13c D2Game+0x8c910: EAX = unit; stack key unit, a, b (ret 0xC). From the disassembly; the batch file's d2_fn_d2game_8C910 follows the
// decompile, which folds the key unit into EAX and drops a stack word.
// A unit without +0x80 answers 0. 0x9BEB0 (EAX = unit) and 0x9BEB0 (EAX = key unit) must agree in their low word, which must not be -1.
// #A4C0 (stack unit) is the player record; its list at +0x98 holds 20-byte nodes {key, a, b, 0, next}, keyed by the key unit's id (+0xC),
// -1 for a null key unit. A miss allocates a node (0xA2E6: EDX = 0x14, stack the file string 0x6FD1BAE8, line 0x6FA, 0) at the head; a
// failed allocation answers 0. A node whose b differs takes (a, b) and answers 1; otherwise 0x8A330 (EAX = node, EDX = a) answers.
#include <cstdlib>
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_8c910(uint32_t unit, uint32_t keyUnit, uint32_t a, uint32_t b, const D2Seam* e)
{
    if (!unit) std::abort();   // retail's fatal exit
    if (!d2rd32(unit + 0x80)) return 0;
    const uint32_t k1 = e->call(0x9BEB0, unit);
    if ((int16_t)k1 == -1) return 0;
    const uint32_t k2 = e->call(0x9BEB0, keyUnit);
    if ((uint16_t)k1 != (uint16_t)k2) return 0;
    const uint32_t rec = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!rec) return 0;
    const uint32_t key = keyUnit ? d2rd32(keyUnit + 0xC) : 0xFFFFFFFFu;
    uint32_t node = d2rd32(rec + 0x98);
    while (node && d2rd32(node) != key) node = d2rd32(node + 0x10);
    if (!node) {
        node = e->call(0xA2E6, 0, 0, 0x14, 0, 0, 0x6FD1BAE8u, 0x6FA, 0);
        if (!node) return 0;
        for (uint32_t i = 0; i < 0x14; i += 4) d2wr32(node + i, 0);
        d2wr32(node + 0x10, d2rd32(rec + 0x98));
        d2wr32(rec + 0x98, node);
        d2wr32(node, key);
    }
    if (d2rd32(node + 8) != b) { d2wr32(node + 4, a); d2wr32(node + 8, b); return 1; }
    return e->call(0x8A330, node, 0, a);
}
