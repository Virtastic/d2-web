// 1.13c D2Win+0x0B4C0 (sub_B4C0): store param and D2CMP #10086(param) into two global pairs
// (DAT_6f9a9e3c/40 and DAT_6f9a9e48/4c).
#include <cstdint>

static uint32_t g_9a9e3c, g_9a9e40, g_9a9e48, g_9a9e4c;

void d2win_sub_b4c0(uint32_t v, uint32_t (*d2cmp10086)(uint32_t)) {
    g_9a9e3c = v;
    g_9a9e40 = d2cmp10086(v);
    g_9a9e48 = v;
    g_9a9e4c = d2cmp10086(v);
}
