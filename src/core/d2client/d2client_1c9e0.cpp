// 1.13c D2Client+0x1C9E0 -- ECX = value, one stack argument (unused). Clears the byte 0x1223F3 and the dword 0x11BF94, stores ECX in 0x11BF98 and returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_1223f0;
__attribute__((weak)) uint32_t g_d2client_11bf94;
__attribute__((weak)) uint32_t g_d2client_11bf98;
namespace d2cl {
uint32_t f1c9e0(GE& e, uint32_t v, uint32_t)
{
    g_d2client_1223f0 &= 0x00ffffffu;
    g_d2client_11bf94 = 0;
    g_d2client_11bf98 = v;
    return 0;
}
}
