// 1.13c D2Client+0x870D0 -- stdcall, one argument. Frees the argument through Storm ordinal 403 (argument, the file name at 0xD4938, line 0x27b, 0) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f870d0(GE& e, uint32_t a)
{
    return e.h(0xbe6c, 0, 0, 0, 0, 0, a, 0xB20D4938u, 0x27b, 0);
}
}
