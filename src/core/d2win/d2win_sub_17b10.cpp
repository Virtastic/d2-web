// 1.13c D2Win+0x17B10 (sub_17B10): store param into the module global at 0x6f9014dc, return 1.
#include <cstdint>

static uint32_t g_d2win_9014dc;   // DAT_6f9014dc

int32_t d2win_sub_17b10(uint32_t v) {
    g_d2win_9014dc = v;
    return 1;
}
