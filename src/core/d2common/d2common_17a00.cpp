// D2Common 0x17A00 (ESI = drlg; ret), retail layout. Finds level 0x4B on the drlg's level list (+0x47C, next +0x1AC,
// id +0x1D0) or makes it through 0x2D9B0; copies its size (+0x1C/+0x20) and the difficulty's two words (+0x24/+0x28,
// difficulty byte drlg+0x450) from level-defs record 0x4B; runs the jungle init and the chain layout on its result,
// then links levels 0x4B and 0x53 through the outdoor and level linkers.
#include <cstdint>
#include <cstring>
#include "d2common_17a00.h"
static uint32_t Rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void Wr(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
// 1.13c D2Common+0x17A00
extern "C" void D2Common_17A00_InitAct3(uint32_t drlg, const D2Common17A00Env* env) {
    uint32_t lv = Rd(drlg + 0x47c);
    while (lv && Rd(lv + 0x1d0) != 0x4b) lv = Rd(lv + 0x1ac);
    if (!lv) lv = env->getLevel(drlg, 0x4b);
    const uint32_t rec = (int32_t)Rd(env->dataTables + 0xc5c) > 0x4b ? env->levelDefs + 0x4b * 0x9c : 0;
    const uint32_t diff = *(const uint8_t*)(uintptr_t)(drlg + 0x450);
    Wr(lv + 0x1c, Rd(rec + 0x24));
    Wr(lv + 0x20, Rd(rec + 0x28));
    Wr(lv + 0x24, Rd(rec + 0x0c + diff * 4));
    Wr(lv + 0x28, Rd(rec + 0x18 + diff * 4));
    env->layoutChain(env->initJungle(lv));
    env->linkOutdoor(drlg, 0x4b, 0x53);
    env->linkLevels(drlg, 0x4b, 0x53);
}
