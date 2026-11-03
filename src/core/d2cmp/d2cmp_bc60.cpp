// 1.13c D2CMP+0xBC60: drain the cell cache. cdecl(), EDI = the cache global.
#include "d2cmp_cache32.h"
using namespace d2cmp32;
// 1.13c D2CMP+0xBC60
extern "C" void d2cmp_bc60(LruCache* cache, const Env& e) {
    e.lruDrain(e.ctx, cache);
}
