// 1.13c D2Common+0x186D0 -- EAX = path info, ESI = walker (D2PathMovStrc: [0] direction, [2] current point
// (x +8, y +0xA), [3] candidate point, [4] dead end, [6] -> next-direction table). Tries the walker's direction,
// then up to three turns from the table, probing each candidate cell with 0x4CE90(room +8, x, y, pattern +0x28,
// mask +0x2C); returns 1 at the first free cell, else 0. [4] is set before the last try. The unit at
// (info+0x30)->+0x30 must be a player or monster (type 0/1); retail reports and exits otherwise.
#include "d2common_186d0.h"
#include <cstdlib>
#include <emscripten.h>

namespace {
inline uint32_t W(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a + o)); }
// gnDirectionsToAdjacentCellOffsets (0x6FDDCA38): 8-byte entries, x at +0, y at +4; read as int16
const int32_t kCellOff[16][2] = { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
                                  {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1} };
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_186D0_StepWalker(uint32_t pInfo, uint32_t* w,
                                                                   const D2CommonWalkEnv* env) {   // 1.13c D2Common+0x186D0
    const uint32_t pattern = W(pInfo, 0x28), mask = W(pInfo, 0x2C), room = W(pInfo, 8);
    const uint32_t unit = W(W(pInfo, 0x30), 0x30);
    if (!unit || (W(unit, 0) != 0 && W(unit, 0) != 1)) abort();   // retail: error 0x122, _exit(-1)
    auto probe = [&]() {
        uint16_t* p = reinterpret_cast<uint16_t*>(w);
        const uint16_t x = uint16_t(kCellOff[w[0]][0] + p[4]);
        const uint16_t y = uint16_t(kCellOff[w[0]][1] + p[5]);
        p[6] = x; p[7] = y;
        return env->chk(room, x, y, pattern, mask);
    };
    w[4] = 0;
    if (!probe()) return 1;
    const int32_t* next = reinterpret_cast<const int32_t*>(uintptr_t(w[6]));
    w[0] = uint32_t(next[w[0]]);
    if (!probe()) return 1;
    w[0] = uint32_t(next[w[0]]);
    if (!probe()) return 1;
    w[4] = 1;
    w[0] = uint32_t(next[w[0]]);
    return probe() ? 0 : 1;
}
