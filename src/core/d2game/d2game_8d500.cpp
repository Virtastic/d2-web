// 1.13c D2Game+0x8d500: EAX = unit, EDI = value, one stack id (ret 4). From the disassembly; the batch file's d2_fn_d2game_8D500 follows
// the decompile, which loses EAX of both callees.
// For a type-1 unit with data (+0x14) whose +0x28 record has a non-zero +0x28, 0xE03A0 (EAX = record +0x30, ECX = +0x28, EDX = +0x2C)
// answers an id (0 otherwise). When it equals the stack id and the data's +0x40 differs from the value, 0x8A330 (EAX = data + 0x3C,
// EDX = value) answers; everything else answers 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_8d500(uint32_t unit, uint32_t edi, uint32_t id, const D2Seam* e)
{
    if (!unit || d2rd32(unit) != 1) return 0;
    const uint32_t mon = d2rd32(unit + 0x14);
    uint32_t got = 0;
    if (mon) {
        const uint32_t rec = d2rd32(mon + 0x28);
        if (rec && d2rd32(rec + 0x28)) got = e->call(0xE03A0, d2rd32(rec + 0x30), d2rd32(rec + 0x28), d2rd32(rec + 0x2C));
    }
    if (got != id || !mon || d2rd32(mon + 0x40) == edi) return 0;
    return e->call(0x8A330, mon + 0x3C, 0, edi);
}
