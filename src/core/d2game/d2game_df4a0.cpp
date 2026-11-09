// 1.13c D2Game+0xDF4A0, from the disassembly. Two paths pick a signed table id, then the common tail (0x6FCFF4FD) bounds-checks it
// against the table count (+0xA80), stores the 0x1A8-byte entry address at *param_4 and returns the id as EAX leaves it.
//  unit path (EAX != 0 and param_3[0x6D] == 0): record = A74E(param_2[0x2C0]); record[0x35] == 0: *out = 0, return 0;
//    id = int16 record[0x9A + 2 * sub11D0(rng, record[0x35])]; the unit path never rolls.
//  list path: param_2[0x10] == 0: *out = 0, return 0; else walk 0x34-byte entries at param_2+0x14 (count byte at +2) with
//    index sub11D0(rng, param_2[0x11]) + 1; id = the entry's first int16; when the id indexes a table whose +0x20 word is >= 0 and whose
//    +0x0E byte passes masks[0x1C], one LCG step (lo * 0x6AC690C5 + hi, 64-bit pair stored back) and (low32 % 100) > param_5 swaps in the +0x20 word.
#include "d2game_df4a0.h"
#include <cstring>
namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
int32_t rd16s(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
}
int32_t d2game_df4a0_port(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t p3, uint32_t p4, uint32_t p5, const D2GameDF4A0Env* e) {
    const uint32_t rng = ecx ? ecx + 0x6c : 0;
    int32_t id;
    if (eax && rd8(p3 + 0x6d) == 0) {
        const uint32_t rec = e->subA74E(rd32(edx + 0x2c0));
        const uint32_t lvl = rd8(rec + 0x35);
        if (!lvl) { wr32(p4, 0); return 0; }
        const uint32_t i = e->sub11D0(rng, lvl);
        id = rd16s(rec + 0x9a + i * 2);
    } else {
        if (!rd8(edx + 0x10)) { wr32(p4, 0); return 0; }
        int32_t rem = (int32_t)e->sub11D0(rng, rd8(edx + 0x11)) + 1;
        const int32_t n = (int32_t)rd8(edx + 0x10);
        uint32_t ent = edx + 0x14;
        int32_t k = 0;
        if (n > 0) {
            do {
                rem -= (int32_t)rd8(ent + 2);
                if (rem <= 0) break;
                ent += 0x34; ++k;
            } while (k < n);
        }
        int32_t cx = rd16s(ent);
        id = cx;
        if (id >= 0 && id < (int32_t)rd32(e->tables + 0xa80)) {
            const uint32_t te = (uint32_t)id * 0x1a8 + rd32(e->tables + 0xa78);
            if (te && rd16s(te + 0x20) >= 0 && (rd8(e->masks + 0x1c) & rd8(te + 0xe))) {
                const uint64_t s = (uint64_t)rd32(rng) * 0x6ac690c5ull + rd32(rng + 4);
                wr32(rng + 4, (uint32_t)(s >> 32)); wr32(rng, (uint32_t)s);
                if ((int32_t)((uint32_t)s % 100) > (int32_t)(p5 & 0xff)) cx = rd16s(te + 0x20);
            }
        }
        id = (int16_t)cx;
    }
    if (id < 0) { wr32(p4, 0); return id; }
    if (id >= (int32_t)rd32(e->tables + 0xa80)) { wr32(p4, 0); return id; }
    wr32(p4, (uint32_t)id * 0x1a8 + rd32(e->tables + 0xa78));
    return id;
}
