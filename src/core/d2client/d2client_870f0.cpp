// 1.13c D2Client+0x870F0 -- stdcall, one argument. Allocates the argument's byte count through Storm ordinal 401 (argument, the file name at 0xD4938, line 0x274, 0) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f870f0(GE& e, uint32_t a)
{
    return e.h(0xbe66, 0, 0, 0, 0, 0, a, 0xB20D4938u, 0x274, 0);
}
}
