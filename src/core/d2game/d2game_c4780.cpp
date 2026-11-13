// 1.13c D2Game+0xC4780 -- EAX = the caller's, EDX = event record (+0 game, +0xC unit, +0x14 type, +0x18 argument word). When (type, argument) is one of 0xAF/0xD7, 0xB1/{0x11E,0x11D}, 0xB2/{0x108,0x107}, 0xC6/0xBE, 0xC7/{0xCC,0xCB}, 0xC8/{0xE7,0xE6}, 0xCA/0x112, 0xD2/{0xF2,0xF1} the worker 0x695F0 (EAX = unit, ESI = type, one stack word = game) runs and EAX leaves as its answer; otherwise EAX = the caller's upper half with the argument word in AX.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4780(GE& e, uint32_t eax, uint32_t, uint32_t ev)
{
    const uint32_t ty = e.rd32(ev + 0x14), w = e.rd32(ev + 0x18) & 0xffff;
    bool call = false;
    switch (ty) {
    case 0xaf: call = w == 0xd7; break;
    case 0xb1: call = w == 0x11e || w == 0x11d; break;
    case 0xb2: call = w == 0x108 || w == 0x107; break;
    case 0xc6: call = w == 0xbe; break;
    case 0xc7: call = w == 0xcc || w == 0xcb; break;
    case 0xc8: call = w == 0xe7 || w == 0xe6; break;
    case 0xca: call = w == 0x112; break;
    case 0xd2: call = w == 0xf2 || w == 0xf1; break;
    }
    if (!call) return (eax & 0xffff0000u) | w;
    return e.h(0x695f0, e.rd32(ev + 0xc), 0, 0, 0, ty, e.rd32(ev));
}
}
