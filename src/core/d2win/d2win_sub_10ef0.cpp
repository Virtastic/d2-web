// 1.13c D2Win+0x10EF0 (sub_10EF0): type-0xB control (list/popup) frame + item-text draw.
// Callees are seams: Fog #10265/#10024 fatal tail, Ordinal_10049/10035/10014 (D2Gfx/D2Win imports),
// 0x12FE0 (font select), 0x12940 (text draw).
#include <cstdint>
#include <unistd.h>

struct D2Win10EF0Env {
    uint32_t gdwBitMask0, gdwBitMask8;                    // gdwBitMasks[0], [2]
    uint8_t (*o10049)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);   // (&DAT_6f922338,0x100,0x3f,0x3f)
    void (*o10035)(int32_t* px, uint32_t v);              // (&x, 0xff)
    void (*o10014)(uint32_t a, int32_t y, int32_t x, uint32_t col, uint32_t b, uint32_t c);
    void (*sub_2FE0)(int32_t font);                       // 0x12FE0
    void (*sub_2940)(const void* text, int32_t y, int32_t centerW, uint32_t shift, int32_t centered); // 0x12940
    void (*fatal)();                                      // Fog #10265 + #10024("")
};

// ctl = ECX (int32 array view of the control). Returns 1 if drawn.
uint32_t d2win_sub_10ef0(int32_t* c, const D2Win10EF0Env& env) {
    if (c[0] != 0xb) { env.fatal(); _exit(-1); }
    uint32_t flags = (uint32_t)c[2];
    if (!(env.gdwBitMask8 & flags) || !(env.gdwBitMask0 & flags)) return 0;
    uint32_t col = env.o10049(0, 0x100, 0x3f, 0x3f);
    int32_t x = c[3] - 5;
    int32_t y = (c[4] - c[6]) - 5;
    env.o10035(&x, 0xff);
    env.o10014(0x3f, y, x, col & 0xff, 0x100, 2);
    env.sub_2FE0(1);
    int32_t n = c[0x10];
    for (int32_t i = 0; i < n; i++) {
        // item i text at c+0x11 + i*0x84 dwords stride (0x84 uint16 units in decompile)
        const uint16_t* t = (const uint16_t*)(c + 0x11) + i * 0x84;
        env.sub_2940(t, c[4] + ((i - n) + 1) * 0x14, 0, ((i != c[0x119]) - 1) & 3, 0);
        n = c[0x10];
    }
    return 1;
}
