// 1.13c D2Game+0xC73A0 -- Act 2 quest callback (ECX = quest node, EDX = event {+0x14 first word, +0x18 second word, +0xC player unit});
// written from the 1.13c disassembly. Second word 0x2C or 0x2D with node state (+0xC) 0 and +9 == 1 runs 0xC7250 (ESI node). Second word 0x28 with
// node data (+0x18) byte +3 == 1 runs 0xC6A30 (ECX = ESI = node); a nonzero low byte registers callback C3750 through 0xDFCC0 (EDI = game) and
// clears the data byte. First word 0x28: a nonzero node word at +0x9C runs the party broadcast 0x33830 (ECX node + 0x1C, EAX player id or -1);
// state 2 becomes 3 with the debug log 0xCBE70 (global dbg). EAX at the return follows retail: the first word, then its low byte replaced by the state
// byte on the 0x2C / 0x2D test, then each callee's result; wsprintfA leaves 0 and the debug-global load leaves its value.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc73a0(HookEnv& e, uint32_t node, uint32_t ev, uint32_t dbg)
{
    const uint32_t a = e.rd32(ev + 0x14), b = e.rd32(ev + 0x18), d = e.rd32(node + 0x18);
    uint32_t eax = a;
    if (b == 0x2C || b == 0x2D) {
        eax = (eax & ~0xFFu) | e.rd8(node + 0xC);
        if (e.rd8(node + 0xC) == 0 && e.rd8(node + 9) == 1) eax = e.h(0xC7250, 0, 0, 0, 0, node);
    }
    if (b == 0x28 && e.rd8(d + 3) == 1) {
        eax = e.h(0xC6A30, 0, node, 0, 0, node);
        if (eax & 0xFF) {
            const uint32_t g = e.rd32(node + 4);
            eax = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC3750, 0, 0);
            e.edi(0xDFCC0, g);
            e.wr8(d + 3, 0);
        }
    }
    if (a == 0x28) {
        if ((uint16_t)e.rd16s(node + 0x9C)) {
            const uint32_t pl = e.rd32(ev + 0xC);
            eax = e.h(0x33830, pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu, node + 0x1C);
        }
        if (e.rd8(node + 0xC) == 2) {
            e.wr8(node + 0xC, 3);
            eax = dbg;
            if (dbg) { char buf[128]; eax = e.h(0xCBE70, 0, 0, e.stk(buf), 0, 0, e.rd32(node + 4), 4); }
        }
    }
    return eax;
}
}
