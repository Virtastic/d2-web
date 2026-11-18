// 1.13c D2CMP+0xBBB0: cell-cache init. stdcall(identity, budget).
#include "d2cmp_cache32.h"
using namespace d2cmp32;
// 1.13c D2CMP+0xBBB0
extern "C" void d2cmp_bbb0(const Globals& g, uint32_t identity, uint32_t budget, const Env& e) {
    std::memset(g.cache, 0, sizeof *g.cache);   // rep stosd x13
    *g.budget = budget;                          // 6FE87F0C
    e.lruInit(e.ctx, g.cache, 0x1000, identity, budget, kDestroyBB60);
    *g.enabled = 1;                              // 6FE87F08
}
