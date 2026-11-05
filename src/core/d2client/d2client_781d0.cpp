// 1.13c D2Client+0x781D0 (77 B): EDX = x offset, EBX = text (the other inputs are globals). y = EDX + [0x11B9A4] + [0xDBC4C] - 0x1E0;
// the span is [0xF9E14] - [0x11B9A0] = s, x = s - 0x5A, widened by (0x5B - width) >> 1 (arithmetic) when the text width from
// 0xD35A (ECX = text) is below 0x5B; the text is drawn through 0xD37E (ECX = text, EDX = x, stack y, 0, 0).
#include <cstdint>
#include <emscripten.h>

struct D2c781D0Env {
    int32_t (*textWidth)(uint32_t text);                                             // 0xD35A (ECX)
    void (*drawText)(uint32_t text, int32_t x, int32_t y, uint32_t z0, uint32_t z1);   // 0xD37E (ECX, EDX, ret 12)
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_781d0(int32_t edx, uint32_t ebx, int32_t g_dbc4c, int32_t g_11b9a4, int32_t g_f9e14,
                                                    int32_t g_11b9a0, const D2c781D0Env* env) {
    const uint32_t y = (uint32_t)edx + ((uint32_t)g_11b9a4 + (uint32_t)g_dbc4c) - 0x1e0u;
    const uint32_t s = (uint32_t)g_f9e14 - (uint32_t)g_11b9a0;
    uint32_t x = s - 0x5au;
    const int32_t span = (int32_t)(s - x + 1u);
    const int32_t w = env->textWidth(ebx);
    if (w < span) x += (uint32_t)(((int32_t)((uint32_t)span - (uint32_t)w)) >> 1);
    env->drawText(ebx, (int32_t)x, (int32_t)y, 0, 0);
}
