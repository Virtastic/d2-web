// src/core/d2win/d2win_104b0.cpp -- 1.13c D2Win.dll+0x104B0, image (type 2) nine-slice render
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win104B0Env {
    // D2Win+0x10280 (dst rect in EAX in retail; src rect and the gfx block on the stack). Rects
    // are {x0, x1, y0, y1}.
    void (*blitRect)(const int32_t* dst, const int32_t* src, uint8_t* gfx);
};

// 1.13c D2Win+0x104B0. The source frame is 256 x 256 cut by the insets L = [0x13] and
// T = [0x14]; the destination box is (x, y - h) .. (x + w, y). Nine blits, row by row.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_104B0(int32_t* ctl, const D2Win104B0Env* env) {
    if (ctl[0] != 2) fatal(0x10F);
    int32_t L = ctl[0x13], T = ctl[0x14];
    int32_t x = ctl[3], y = ctl[4], w = ctl[5], h = ctl[6];
    uint8_t gfx[0x48];
    std::memset(gfx, 0, sizeof gfx);
    wr32(gfx, 0x34, (uint32_t)ctl[1]);
    const int32_t sx[4] = {0, L, 0x100 - L, 0x100};
    const int32_t sy[4] = {0, T, 0x100 - T, 0x100};
    const int32_t dx[4] = {x, x + L, x + w - L, x + w};
    const int32_t dy[4] = {y - h, y - h + T, y - T, y};
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i) {
            int32_t src[4] = {sx[i], sx[i + 1], sy[j], sy[j + 1]};
            int32_t dst[4] = {dx[i], dx[i + 1], dy[j], dy[j + 1]};
            env->blitRect(dst, src, gfx);
        }
    return 1;
}
