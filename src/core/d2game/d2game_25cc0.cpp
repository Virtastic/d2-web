// 1.13c D2Game+0x25cc0: ECX = game, EDX = unit; two ignored stack words. Builds a 23-dword request {[0] 0x21, [1] unit, [4] 0x141,
// [5],[6] the unit position, [12] #A42A(unit, 0xc, 0), [21] 0x6FC61F50} and walks the four neighbours (0,-1),(1,0),(0,1),(-1,0):
// the position is re-read each step, [7],[8] = position + step, and 0x6F930 (ECX = game, EDX = request) runs twice with [22] = 0, 1.
#include "d2game_f1pos.h"
extern "C" void d2g_25cc0(uint32_t game, uint32_t unit, const D2Seam* e)
{
    uint32_t q[23] = {};
    q[0] = 0x21; q[1] = unit; q[4] = 0x141;
    q[5] = f1PosX(unit, e);
    q[6] = f1PosY(unit, e);
    q[12] = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xc, 0);
    q[21] = 0x6fc61f50;
    static const int32_t dx[4] = { 0, 1, 0, -1 }, dy[4] = { -1, 0, 1, 0 };
    for (int i = 0; i < 4; i++) {
        const uint32_t x = f1PosX(unit, e), y = f1PosY(unit, e);
        q[7] = dx[i] + x; q[8] = dy[i] + y;
        for (uint32_t k = 0; k < 2; k++) {
            q[22] = k;
            e->call(0x6F930, 0, game, (uint32_t)(uintptr_t)q);
            e->blk(0x6F930, q, 23);
        }
    }
}
