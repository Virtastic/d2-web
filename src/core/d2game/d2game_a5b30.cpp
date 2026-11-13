// 1.13c D2Game+0xA5B30 -- ECX game, EDX unit, one stack word ctl (ret 4). No result.
// With ctl[+0x18] non-zero: a unit (id = unit[+1 word], valid when 0 <= id < dt[+0xA80], dt = [D2Game+0xF829C]) whose record (dt[+0xA78] + id * 0x1A8) byte +0xD
// has a bit in bm[1] (bm = [D2Game+0xF8BE0]) goes to the roll below; otherwise 0x1560(EAX id (-1 for no unit), EDX 4) non-zero: 0xA496(unit, 0), then the request
// {4, unit, ctl[8], 0, 0, byte +0x15 = 0x64 (0x65 when the unit has a 0x1320(EAX id) record whose byte +0x104 has a bit in bm[4]), 0...} (8 words) goes to 0x75E10(game, &req, 1). Done.
// Roll (ctl[+0x18] zero, or 0x1560 zero, or the bit set): seed [unit+0x20/0x24] steps by *0x6AC690C5 + hi (low word % 100 < 0x14): 0xF1B10(AL 3, ECX 0x64; game, unit), else 0xF22C0(EAX game, ESI unit; 10).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fa5b30(GE& e, uint32_t game, uint32_t unit, uint32_t ctl, uint32_t dt, uint32_t bm)
{
    bool roll = true;
    if (e.rd32(ctl + 0x18)) {
        roll = false;
        int32_t id = -1;
        bool bit = false;
        if (unit) {
            id = (int32_t)e.rd32(unit + 4);
            if (id >= 0 && id < (int32_t)e.rd32(dt + 0xA80)) {
                const uint32_t rec = (uint32_t)id * 0x1A8 + e.rd32(dt + 0xA78);
                if (rec) bit = (e.rd8(rec + 0xD) & e.rd32(bm + 4)) != 0;
            }
        }
        if (bit) roll = true;
        else if (!e.h(0x1560, (uint32_t)id, 0, 4, 0, 0)) roll = true;
        else {
            e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
            uint32_t st[8] = {4, unit, e.rd32(ctl + 8), 0, 0, 0, 0, 0};
            uint8_t bl = 0x64;
            if (unit) {
                const uint32_t m = e.h(0x1320, e.rd32(unit + 4));
                if (m && (e.rd8(m + 0x104) & e.rd32(bm + 0x10))) bl = 0x65;
            }
            st[5] = (uint32_t)bl << 8;
            e.h(0x75E10, 0, 0, 0, 0, 0, game, (uint32_t)(uintptr_t)st, 1);
            e.blk(0x75E10, (uint32_t)(uintptr_t)st, 8);
            return;
        }
    }
    (void)roll;
    const uint64_t s = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5ull + e.rd32(unit + 0x24);
    e.wr32(unit + 0x24, (uint32_t)(s >> 32)); e.wr32(unit + 0x20, (uint32_t)s);
    if ((uint32_t)s % 100 < 0x14) e.h(0xF1B10, 3, 0x64, 0, 0, 0, game, unit);
    else e.h(0xF22C0, game, 0, 0, 0, unit, 10);
}
}
