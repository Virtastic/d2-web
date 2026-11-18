// 1.13c D2Win+0x10EF0 (sub_10EF0): type-0xB control (list/popup) frame + item-text draw.
// Callees are seams: Fog #10265/#10024 fatal tail, 0x7762 (palette index), 0x784C (rect clamp), 0x7822 (fill),
// 0x12FE0 (font select), 0x12940 (text draw; EAX = x).
#include <cstdint>
#include <unistd.h>

struct D2Win10EF0Env {
    uint32_t gdwBitMask0, gdwBitMask8;                    // gdwBitMasks[0], [2]
    uint8_t (*o7762)(const void* pal, uint32_t a, uint32_t b, uint32_t c, uint32_t d);   // (&DAT_6f922338,0x100,0x3f,0x3f,0x3f)
    void (*o784c)(int32_t* rect, uint32_t v);             // rect {left, top, right, bottom}, 0xff
    void (*o7822)(int32_t left, int32_t bottom, int32_t right, int32_t top, uint32_t col, uint32_t mode);
    void (*sub_2FE0)(int32_t font);                       // 0x12FE0
    void (*sub_2940)(int32_t x, const void* text, int32_t y, int32_t centerW, uint32_t shift, int32_t centered); // 0x12940
    void (*fatal)();                                      // Fog #10265 + #10024("")
};

// ctl = ECX (int32 array view of the control). Returns 1 if drawn.
extern "C" uint32_t d2win_sub_10ef0(int32_t* c, const D2Win10EF0Env& env) {
    if (c[0] != 0xb) { env.fatal(); _exit(-1); }
    uint32_t flags = (uint32_t)c[2];
    if (!(env.gdwBitMask8 & flags) || !(env.gdwBitMask0 & flags)) return 0;
    static const uint8_t pal = 0;
    uint32_t col = env.o7762(&pal, 0x100, 0x3f, 0x3f, 0x3f) & 0xff;
    int32_t rect[4] = { c[3] - 5, (c[4] - c[6]) - 5, c[3] + c[5] + 5, c[4] + 5 };
    env.o784c(rect, 0xff);
    env.o7822(rect[0], rect[3], rect[2], rect[1], col, 2);
    env.sub_2FE0(1);
    const uint8_t* t = (const uint8_t*)c + 0x44;
    for (int32_t i = 0; i < c[0x10]; i++, t += 0x84)
        env.sub_2940(c[3], t, c[4] + ((i - c[0x10]) + 1) * 0x14, 0, ((i != c[0x119]) - 1) & 3, 0);
    return 1;
}
