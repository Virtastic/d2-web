#include <cstdint>

// CRT new-handler hook (DAT_6fdef964); null unless a handler is installed.
static int32_t (*g_d2common_newh_8b00)(uint32_t size) = nullptr;

// 1.13c D2Common+0x08B00
int32_t d2common_callnewh_8b00(uint32_t size)
{
    if (g_d2common_newh_8b00 && g_d2common_newh_8b00(size) != 0) return 1;
    return 0;
}
