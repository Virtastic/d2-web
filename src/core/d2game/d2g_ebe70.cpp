// 1.13c D2Game+0xEBE70 -- stat list bit-pack encoder. Stdcall, three stack words (ret 0xC): unit, ppCursor, pEnd.
// Seam form of D2GAME_EBE70 (d2game_batch_e4280b.cpp), written from the disassembly: the stat id / value writer is ONE stack
// bit-writer {0xA2B6 init(bw, cursor+2, end-(cursor+2)), 0xA2CE write(bw, value, bits), 0xA2E0 finish(bw) -> byte length};
// 0xA4C6 = Ordinal 10658 (list, out, 0x200) fills (u16 aux, u16 id, i32 value) entries and returns their count.
// dt = *sgptDataTables (stat rows 0x144 stride at +0xBCC, count at +0xBD4). The sign-clamp test reads gdwBitMasks[5] (0x20).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ebe70(uint32_t unit, uint32_t ppCursor, uint32_t pEnd, uint32_t dt, const D2Seam* e)
{
    const uint32_t start = d2rd32(ppCursor) + 2;
    if (start > pEnd) return 1;
    d2wr16(start - 2, 0x6667);
    uint32_t bw[3] = {};
    const uint32_t b = (uint32_t)(uintptr_t)bw;
    e->call(0xA2B6, 0, 0, 0, 0, 0, b, start, pEnd - start);
    uint32_t ent[0x400] = {};
    uint32_t n = 0;
    const uint32_t list = d2rd32(unit + 0x5C);
    if (list) {
        n = e->call(0xA4C6, 0, 0, 0, 0, 0, list, (uint32_t)(uintptr_t)ent, 0x200);
        if ((int32_t)n > 0) {
            for (int32_t k = 0; k < (int32_t)n; ++k) {
                const uint32_t aux = ent[k * 2] & 0xFFFF, id = ent[k * 2] >> 16;
                int32_t val = (int32_t)ent[k * 2 + 1];
                if ((int32_t)id >= (int32_t)d2rd32(dt + 0xBD4)) continue;
                const uint32_t row = id * 0x144 + d2rd32(dt + 0xBCC);
                if (!row) continue;
                const uint32_t bits = d2rd8(row + 0xA);
                if (!bits) continue;
                if (bits < 0x20) {
                    if (d2rd8(row + 5) & 0x20) {
                        int32_t lo = (int32_t)(~0u << ((bits - 1) & 31)), hi = (int32_t)((1u << ((bits - 1) & 31)) - 1);
                        if (val < lo) val = lo;
                        if (val >= hi) val = hi;
                    } else {
                        const int32_t hi = (int32_t)((1u << (bits & 31)) - 1);
                        if (val <= 0) val = 0;
                        if (val >= hi) val = hi;
                    }
                }
                e->call(0xA2CE, 0, 0, 0, 0, 0, b, id, 9);
                if (d2rd8(row + 0xB)) e->call(0xA2CE, 0, 0, 0, 0, 0, b, aux, d2rd8(row + 0xB));
                e->call(0xA2CE, 0, 0, 0, 0, 0, b, (uint32_t)val, bits);
            }
        }
    }
    e->call(0xA2CE, 0, 0, 0, 0, 0, b, 0x1FF, 9);
    const uint32_t len = e->call(0xA2E0, 0, 0, 0, 0, 0, b);
    d2wr32(ppCursor, len + start);
    return 0;
}
