// 1.13c D2CMP+0xBD70: cell-cache shutdown. ECX this (only forwarded to 0x9880), stack flush.
#include "d2cmp_cache32.h"
using namespace d2cmp32;
// 1.13c D2CMP+0xBD70
extern "C" void d2cmp_bd70(const Globals& g, uint32_t ecx, uint32_t flush, const Env& e) {
    if (!*g.enabled) { e.registryShutdown(e.ctx, ecx); return; }  // 6FE87F08 clear: ECX passes through
    if (flush) e.lruDrain(e.ctx, g.cache);
    std::memset(g.cache, 0, sizeof *g.cache);   // rep stosd x13
    *g.budget = 0;
    e.registryShutdown(e.ctx, 0);               // ECX is 0 after the rep stosd
}
