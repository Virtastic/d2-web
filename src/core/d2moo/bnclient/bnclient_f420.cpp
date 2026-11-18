// 1.13c BNClient+0xF420 -- hands the module record at image offset 0x1E9F0 to the initialiser at 0x155D0 (one stack word,
// callee pops) and returns the record's address.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace bnclient {
uint32_t ff420(GE& e)
{
    const uint32_t rec = 0xB2000000u | 0x1E9F0;   // a module address, as the capture records it
    e.h(0x155D0, 0, 0, 0, 0, 0, rec);
    return rec;
}
}
