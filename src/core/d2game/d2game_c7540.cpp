// 1.13c D2Game+0xC7540 -- Act 2 "killed the quest monster" callback (ECX = quest node, EDX = event {+0 game, +8 unit id}); written from the 1.13c disassembly.
// A node with a zero byte at +9 does nothing (EAX 0). Else (debug log 0xCBE70 around the state change when the global dbg is set) the node state (+0xC) becomes 4,
// the game's quest holder (game +0x10F4, +8 must be set) gets flag (holder +0xC, node +0xE0, 0xD) through 0xA57A, node data (+0x18) byte 0 = 1 and
// dword +4 = 0xA478(unit id); the callbacks C6070, C5560 (then data +0xC = 0) and C54A0 are registered through 0xDFCC0 (EDI = game); a clear
// data byte +0xA is set and 0x33AF0(EBX 0xC; node, C6010) runs; the result is data dword +0xC, whose count loop (0x11A70 per entry) is not reachable with recorder callees.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc7540(HookEnv& e, uint32_t eax, uint32_t node, uint32_t ev, uint32_t dbg)
{
    if (!e.rd8(node + 9)) return eax & ~0xFFu;   // retail: mov al,[node+9] leaves the entry EAX upper bytes
    if (dbg) e.h(0xCBE70, 0, 0, 0xB2000000u | 0xFB338, 0, 0, e.rd32(node + 4), 4);
    e.wr8(node + 0xC, 4);
    if (dbg) { char buf[128]; e.h(0xCBE70, 0, 0, e.stk(buf), 0, 0, e.rd32(node + 4), 4); }
    const uint32_t game = e.rd32(ev);
    const uint32_t hd = e.rd32(game + 0x10F4);
    e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(hd + 0xC), e.rd32(node + 0xE0), 0xD);
    const uint32_t d = e.rd32(node + 0x18);
    e.wr32(node + 0xA8, 0);
    e.wr8(d, 1);
    e.wr32(d + 4, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ev + 8)));
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC6070, 0, e.rd32(ev + 8)); e.edi(0xDFCC0, game);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC5560, 0, e.rd32(ev + 8)); e.edi(0xDFCC0, game);
    e.wr32(d + 0xC, 0);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC54A0, 0, e.rd32(ev + 8)); e.edi(0xDFCC0, game);
    if (!e.rd8(d + 0xA)) {
        e.wr8(d + 0xA, 1);
        e.h(0x33AF0, 0, 0, 0, 0xC, 0, node, 0xB2000000u | 0xC6010);
    }
    const int32_t n = (int32_t)e.rd32(d + 0xC);
    if (n) {
        e.wr32(e.rd32(ev + 8) + 0xB8, 0x20737361);
        for (int32_t i = 0; i < (int32_t)e.rd32(d + 0xC); ++i) { char loc[16]; e.h(0x11A70, 0, 0, 0, e.rd32(node + 4), 0, e.rd32(ev + 8), 2, e.stk(loc), 0); }
    }
    return (uint32_t)n;
}
}
