// 1.13c D2Game+0xE51C0 -- ECX = quest node, EDX = event (+8 and +0xC records, a record's +4 = its type word, +0xC = its id); no stack argument. When the +8 record exists with type word 0xFE and the node's +0x18 data has state byte (+0xC) 1: the +0xC record's id (-1 when none) is compared with the data's +0x10 word; on a match, unless that record has type 5 or 6, the worker 0xDFFD0(0x43, record) runs, and the data's state byte is cleared. Returns what the instruction stream leaves in EAX: the +8 record, the +0xC record, its type word, or the worker's answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe51c0(GE& e, uint32_t node, uint32_t ev)
{
    const uint32_t r1 = e.rd32(ev + 8);
    if (!r1 || (e.rd16s(r1 + 4) & 0xffff) != 0xfe) return r1;
    const uint32_t d = e.rd32(node + 0x18);
    if (e.rd8(d + 0xc) != 1) return r1;
    const uint32_t r2 = e.rd32(ev + 0xc);
    const uint32_t id = r2 ? e.rd32(r2 + 0xc) : 0xffffffffu;
    if (id != e.rd32(d + 0x10)) return r2;
    uint32_t a = r2;
    if (r2) a = e.rd32(r2 + 4);
    if (!r2 || (a != 6 && a != 5)) a = e.h(0xDFFD0, 0, 0, 0, 0, 0, 0x43, e.rd32(ev + 0xc));
    e.wr8(d + 0xc, 0);
    return a;
}
}
