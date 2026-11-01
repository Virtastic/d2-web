// 1.13c D2Common+0x15840 -- EAX = level; from each of the four grid corners, flag the open cells (bit 0 clear)
// of every row until a row starts on a blocked cell. Retail-layout level: +0x14 -> outdoor grid (cells +0x2C,
// row offsets +0x30, width +0x5C, height +0x60). The flag goes through the cell-OR pointer at 0x6FDEB244 (0x36C10).
#include <cstdint>
#include <emscripten.h>

namespace {
inline uint32_t W(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a + o)); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_15840_FlagCornerRuns(uint32_t pLevel) {   // 1.13c D2Common+0x15840
    const uint32_t od = W(pLevel, 0x14);
    // {from the right edge, from the bottom edge, step x, step y}
    static const int32_t k[4][4] = { {0, 0, 1, 1}, {1, 0, -1, 1}, {0, 1, 1, -1}, {1, 1, -1, -1} };
    for (int32_t i = 0; i < 4; ++i) {
        const int32_t x0 = k[i][0] ? int32_t(W(od, 0x5C)) - 1 : 0;
        int32_t y = k[i][1] ? int32_t(W(od, 0x60)) - 1 : 0;
        auto cell = [&](int32_t x) { return reinterpret_cast<uint32_t*>(uintptr_t(W(od, 0x2C)) +
                                     uint32_t(int32_t(W(W(od, 0x30), uint32_t(y) * 4)) + x) * 4); };
        while (!(*cell(x0) & 1)) {
            for (int32_t x = x0; !(*cell(x) & 1); x += k[i][2]) *cell(x) |= 0x100;
            y += k[i][3];
        }
    }
}
