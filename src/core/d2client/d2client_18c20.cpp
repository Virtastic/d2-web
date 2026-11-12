// 1.13c D2Client+0x18C20 -- EDX = the unit (nothing happens without one), four stack words: a source unit, a packed dword (state id in the high word, a skill or item id in the low word), an unused word and a flag. State 0x59 runs 0x18BC0 (EDI = the unit, stack: the flag); state 0x5A runs 0x189A0 (EAX = the unit, EDX = the flag). States 0x61 and 0x6B, for a unit of type 0: D2Common ordinal 10708 (ECX = the unit, EDX = the id, stack -1) is asked; with a clear flag a non-zero answer goes to ordinal 10306 (unit, answer, 0) and, when that is not positive, ordinal 10773 (unit, id, the file name at 0xD6F7C, 0xDD) runs; with a set flag and a zero answer the record 0x15B0 (EAX = the id) gives is looked up and, for state 0x61 or when the signed record byte +0xC equals the unit dword +4, ordinal 10302 (unit, id, 0, 0, the file name, 0xE8) runs. State 0xCC, for a unit of type 0: the id is split with the shift and mask at +0xC6C and +0xC70 of the data tables (reached through the import 0xCE474); with a source unit, D2Common ordinal 10973 (source, 0xCC, id) gives a byte; when it is non-zero and ordinal 11160 (source, 0) answers the unit, ordinal 10029 runs with (unit, the source dword +0xC, id shifted, id masked, the byte, 0), otherwise with (..., 0, 1). Every other state does nothing. The result register carries the last callee answer, or the packed dword (the low word for state 0xCC without a source) when nothing ran.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f18c20(GE& e, uint32_t ea, uint32_t, uint32_t unit, uint32_t a1, uint32_t a2, uint32_t, uint32_t a4, uint32_t dt)
{
    if (!unit) return ea;
    const uint32_t st = a2 >> 16, id = a2 & 0xffff;
    if (st == 0x59) {
        const uint32_t r = e.h(0x18BC0, 0, 0, 0, 0, 0, a4);
        e.edi(0x18BC0, unit);
        return r;
    }
    if (st == 0x5a) return e.h(0x189A0, unit, 0, a4);
    if (st == 0x61 || st == 0x6b) {
        if (e.rd32(unit)) return a2;
        const uint32_t r = e.h(0xC4EA, 0, unit, id, 0, 0, 0xffffffffu);
        if (!a4) {
            if (!r) return 0;
            const uint32_t n = e.h(0xC718, 0, 0, 0, 0, 0, unit, r, 0);
            if ((int32_t)n > 0) return n;
            return e.h(0xC706, 0, 0, 0, 0, 0, unit, id, 0xB20D6F7Cu, 0xdd);
        }
        if (r) return r;
        const uint32_t rec = e.h(0x15B0, id);
        if (st != 0x61) {
            if (!rec) return 0;
            const uint32_t al = (uint32_t)(int32_t)(int8_t)(e.rd32(rec + 0xc) & 0xff);
            if (al != e.rd32(unit + 4)) return al;
        }
        const uint32_t s[6] = { unit, id, 0, 0, 0xB20D6F7Cu, 0xe8 };
        const uint32_t v = e.h(0xC550, 0, 0, 0, 0, 0, s[0], s[1], s[2], s[3], s[4]);
        e.blk(0xC550, s, 6);
        return v;
    }
    if (st == 0xcc) {
        if (e.rd32(unit)) return a2;
        const uint32_t T = e.rd32(dt);
        const uint32_t hi = id >> (e.rd32(T + 0xc6c) & 31), lo = e.rd32(T + 0xc70) & id;
        if (!a1) return id;
        const uint32_t own = e.rd32(a1 + 0xc);
        const uint32_t lvl = e.h(0xC1C6, 0, 0, 0, 0, 0, a1, 0xcc, id) & 0xff;
        const bool mine = lvl && e.h(0xC51A, 0, 0, 0, 0, 0, a1, 0) == unit;
        const uint32_t s[6] = { unit, own, hi, lo, mine ? lvl : 0, mine ? 0u : 1u };
        const uint32_t v = e.h(0xCFE2, 0, 0, 0, 0, 0, s[0], s[1], s[2], s[3], s[4]);
        e.blk(0xCFE2, s, 6);
        return v;
    }
    return a2;
}
}
