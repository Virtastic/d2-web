// 1.13c D2Game+0xC4690 -- ECX = game, EDX = event record (+0 game, +8 / +0xC records whose +4 word is a class id or -1 when absent, +0x14 value). Class (record +8) 0xAF, 0xC6, 0xCA map to 7; 0xB1 to 1; 0xB2 to 3; 0xC7 to 2; 0xC8 to 4; 0xD2 to 0; any other class ends the call (EAX = the +0xC record pointer). Otherwise 0x69590 (EAX = the +0xC record, ESI = class, one stack word = event game) answering non-zero ends the call with that answer; else the worker 0x33EB0 runs with EAX = 1 when the +0xC record's class equals the mapped number (else 0), ECX = game and the stack words (+0x14 value, class); EAX leaves as its answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4690(GE& e, uint32_t game, uint32_t ev)
{
    const uint32_t p8 = e.rd32(ev + 8), pc = e.rd32(ev + 0xc);
    const uint32_t ty = p8 ? e.rd32(p8 + 4) : 0xffffffffu, ty2 = pc ? e.rd32(pc + 4) : 0xffffffffu;
    uint32_t want;
    switch (ty) {
    case 0xaf: case 0xc6: case 0xca: want = 7; break;
    case 0xb1: want = 1; break;
    case 0xb2: want = 3; break;
    case 0xc7: want = 2; break;
    case 0xc8: want = 4; break;
    case 0xd2: want = 0; break;
    default: return pc;
    }
    const uint32_t v = e.h(0x69590, pc, 0, 0, 0, ty, e.rd32(ev));
    if (v) return v;
    return e.h(0x33eb0, ty2 == want ? 1u : 0u, game, 0, 0, 0, e.rd32(ev + 0x14), ty);
}
}
