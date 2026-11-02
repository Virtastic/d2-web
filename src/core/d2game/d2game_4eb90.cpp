// 1.13c D2Game+0x4EB90: ecx = p, edx = q, one ignored stack word (ret 4). Calls 0xE0460 (eax = p, 1), #10 (thunk 0xA3CA: p, q, 0); a null p reports through
// 0xA2DA / 0xA2C2 and ends the process (0x2A3C(-1)); then 0xEFC60.
#include "d2game_seam4e.h"
extern "C" void d2game_4eb90(uint32_t p, uint32_t q, const D2Seam* e)
{
    e->call(0xE0460, p, 0, 0, 0, 0, 1);
    e->call(0xA3CA, 0, 0, 0, 0, 0, p, q, 0);
    if (!p) {
        const uint32_t m = e->call(0xA2DA, 0, 0, 0, 0, 0, 0xA2A);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, m);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return;
    }
    e->call(0xEFC60);
}
