// src/core/d2win/d2win_sub_fee0.cpp -- 1.13c D2Win+0x0FEE0 from the cached Ghidra decompile.
#include <cstdint>
#include <unistd.h>

struct D2WinFEE0Env {
    void (*fatal)(uint32_t stringId);                                  // Fog #10265(0xb6) -> #10024("", text)
    const uint32_t* bitMasks;                                          // gdwBitMasks
    void (*drawCel)(int32_t* cel, int32_t x, int32_t y, uint32_t color, int32_t mode, int32_t z);  // Ordinal_10041
};

// 1.13c D2Win+0x0FEE0 (sub_FEE0): draw a type-2 (image) widget tiled in 256px cels.
// w = ECX dword array. Returns 0 when the widget lacks bitMasks[0]/[2] flags, else 1.
uint32_t d2win_sub_fee0(int32_t* w, const D2WinFEE0Env& e) {
    if (w[0] != 2) { e.fatal(0xb6); _exit(-1); }
    if (!(e.bitMasks[2] & (uint32_t)w[2]) || !(e.bitMasks[0] & (uint32_t)w[2])) return 0;
    if (w[1]) {
        int32_t y = w[4], x0 = w[3], h = w[6];
        int32_t cols = (w[5] + 0xff) >> 8;
        int32_t cel[18] = {0};
        cel[13] = w[1];          // local_14
        cel[0] = w[0x10];
        int32_t rows = ((h + 0xff) >> 8) - 1;
        if (rows > 0) {
            y = y - h + 0x101;
            for (int32_t r = rows; r; r--) {
                int32_t x = x0;
                for (int32_t c = cols; c > 0; c--) { e.drawCel(cel, x, y, 0xffffffff, w[0x11], 0); cel[0]++; x += 0x100; }
                y += 0x100;
            }
        }
        if (rows >= 0)
            for (int32_t c = cols; c > 0; c--) { e.drawCel(cel, x0, w[4], 0xffffffff, w[0x11], 0); cel[0]++; x0 += 0x100; }
    }
    return 1;
}
