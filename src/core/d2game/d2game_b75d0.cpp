// 1.13c D2Game+0xB75D0: EAX = target, stack (stdcall, ret 0x14) = (mon, unit, count, medium, far). Fills a 16-slot weight table and picks a slot
// with the unit's LCG (+0x20/+0x24). Transcribed from the disassembly: the Ghidra decompile shows three stack words and puts the later table
// writes two slots low, so the slot numbers here are the frame offsets (esp+0x10 + 4*slot).
// Slot 1 = base + 100 - Ord11027(unit); base = 100, or 0x7D when mon+0x6D is zero. Slots 2/3/4/6 = 0x14, 8 = 0x50, 9 = 0x46.
// count < 2 (signed): slot 2 = 0x2D, slot 10 = 0x19. Ord10507(target) -> 0xB6C90; when zero, Ord10828(target) -> 0xB6C90 again.
// A hit: medium != 0 takes the second row, else slots 13 and 14 = 0x32. No hit and medium == 0 leaves the table. The second row: slots 2 and 6 = 0,
// slot 3 = 0x23, slots 11 and 13 = 0x19. far != 0: slot 5 = 0x3C.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_b75d0(uint32_t target, uint32_t mon, uint32_t unit, uint32_t count, uint32_t medium, uint32_t far, const D2Seam* e)
{
    int32_t w[16] = {0};
    w[2] = w[3] = w[4] = w[6] = 0x14; w[8] = 0x50; w[9] = 0x46;
    int32_t base = d2rd8(mon + 0x6D) == 0 ? 0x7D : 100;
    if ((int32_t)count < 2) { w[2] = 0x2D; w[10] = 0x19; }
    w[1] = base + (100 - (int32_t)e->call(0xA718, 0, 0, 0, 0, 0, unit));
    e->call(0xA3B2, 0, 0, 0, 0, 0, target);
    bool hit = e->call(0xB6C90) != 0;
    if (!hit) {
        e->call(0xA3AC, 0, 0, 0, 0, 0, target);
        hit = e->call(0xB6C90) != 0;
    }
    if (hit || medium) {
        if (medium) { w[2] = 0; w[6] = 0; w[3] = 0x23; w[11] = 0x19; w[13] = 0x19; }
        else { w[14] = 0x32; w[13] = 0x32; }
    }
    if (far) w[5] = 0x3C;
    int32_t sum = 0;
    for (int i = 0; i < 16; i++) sum += w[i];
    uint32_t roll = 0;
    if (sum > 0) {
        uint64_t v = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5ull + d2rd32(unit + 0x24);
        d2wr32(unit + 0x20, (uint32_t)v); d2wr32(unit + 0x24, (uint32_t)(v >> 32));
        roll = ((uint32_t)sum & ((uint32_t)sum - 1)) == 0 ? ((uint32_t)sum - 1) & (uint32_t)v : (uint32_t)v % (uint32_t)sum;
    }
    int32_t acc = 0;
    for (uint32_t i = 0; i < 16; i++) { acc += w[i]; if ((int32_t)roll < acc) return i; }
    return 1;
}
