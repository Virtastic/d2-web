// 1.13c D2Client+0x135E0 -- build the 6x6 floor-vertex records (12 bytes each: 4 colour bytes, x, y).
// Recovered from the disassembly (the decompile loses the stack arguments): stdcall, ret 0x18:
//   arg1 rec        output records, 36 x 12 bytes (row-major, row outer)
//   arg2 colBase    added to the column for the world x (<< 16) and for the null-context fill
//   arg3 rowBase    added to the row for the world y (<< 16)
//   arg4 height     tile height word (0x13A00 passes uint16 sub+4), handed to the projector (Ordinal 10023); 0 selects the light-sampling branch
//   arg5 mode       (0x13A00 passes DAT 0x6FBCC414) 1: x adjust = -(width / 4), 2: x adjust = +(width / 4), else 0 (width = [0x6FB8BC48])
//   arg6 cornersOnly (0x13A00 passes D2Gfx 10047) nonzero: only the four corner cells (col, row in {0, 5}) are written
// Before the loops: b0 = Ordinal_10731(color) (via 0xC5A4), (b1, b3, b2) = Ordinal_10189(color) (via 0xC5B6, out
// pointers in that order), color = [0x6FBCC3B8]. A nonzero height writes rec[0] = b0, rec[1] = b1, rec[2] = b3,
// rec[3] = b2 after adding the x adjust to rec.x; the zero-height branch calls 0x6FB58230 (ECX = (colBase + col) * 8,
// EAX = (rowBase + row) * 8, the four colour byte pointers) after the x adjust instead.
#include <cstdint>
#include <emscripten.h>

struct D2ClientFloorGrid135E0Env {
    uint8_t (*color_byte0)(uint32_t color);                                          // 0xC5A4 / Ordinal 10731
    void (*color_split)(uint32_t color, uint8_t* o1, uint8_t* o3, uint8_t* o2);      // 0xC5B6 / Ordinal 10189
    void (*project)(int32_t x16, int32_t y16, int32_t height, int32_t* px, int32_t* py);  // 0xD084 / Ordinal 10023
    void (*null_fill)(int32_t col8, int32_t row8, uint8_t* c0, uint8_t* c1, uint8_t* c2, uint8_t* c3);  // 0xA8230
    uint32_t color;                                                                  // [0x6FBCC3B8]
    int32_t screen_width;                                                            // [0x6FB8BC48]
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_135e0(uint8_t* rec, int32_t colBase, int32_t rowBase, int32_t height,
                                                     int32_t mode, int32_t cornersOnly,
                                                     const D2ClientFloorGrid135E0Env* env) {
    const uint8_t b0 = env->color_byte0(env->color);
    uint8_t b[4] = {b0, 0, 0, 0};
    int32_t xadj = 0;
    if (mode == 1 || mode == 2) {
        const int32_t w = env->screen_width;
        const int32_t q = (w + ((w >> 31) & 3)) >> 2;
        xadj = mode == 1 ? -q : q;
    }
    env->color_split(env->color, &b[1], &b[3], &b[2]);
    for (int32_t row = 0; row < 6; ++row) {
        for (int32_t col = 0; col < 6; ++col, rec += 0xC) {
            if (cornersOnly != 0 && !((col == 0 || col == 5) && (row == 0 || row == 5))) continue;
            int32_t x = 0, y = 0;
            env->project((colBase + col) << 16, (rowBase + row) << 16, height, &x, &y);
            x += xadj;
            if (height != 0) {
                rec[0] = b[0]; rec[1] = b[1]; rec[2] = b[3]; rec[3] = b[2];
            }
            for (int i = 0; i < 4; ++i) { rec[4 + i] = (uint8_t)(x >> (8 * i)); rec[8 + i] = (uint8_t)(y >> (8 * i)); }
            if (height == 0) env->null_fill((colBase + col) * 8, (rowBase + row) * 8, rec, rec + 1, rec + 2, rec + 3);
        }
    }
}
