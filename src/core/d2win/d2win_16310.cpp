// 1.13c D2Win+0x16310 -- stdcall(msg): type-8 (timer) control; a message other than 0x102 whose msg[2] is 0x20 calls the control's handler (+0x34) and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2w {
uint32_t f16310(GE& e, uint32_t* msg)
{
    uint32_t* c = (uint32_t*)(uintptr_t)msg[0];
    if (c[0] != 8) abort();
    if (msg[1] == 0x102 || msg[2] != 0x20) return 0;
    if (c[0xd]) e.hs(c[0xd] & 0xFFFFFFu, 0, 0, 0, 0, 0, { (uint32_t)(uintptr_t)msg }, 0, 1);
    return 1;
}
}
