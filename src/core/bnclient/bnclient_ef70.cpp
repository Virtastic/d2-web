// 1.13c Bnclient+0xEF70 -- no arguments: runs the worker 0xC4A0 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace bnc {
uint32_t fef70(GE& e, uint32_t)
{
    e.h(0xC4A0);
    return 1;
}
}
