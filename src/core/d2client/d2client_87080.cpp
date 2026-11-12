// 1.13c D2Client+0x87080 -- stdcall, two arguments (source, length). A pending buffer (the dword 0x11BF44) is freed through Storm ordinal 403 (buffer, the file name at 0xD4938, line 0x283, 0). Storm ordinal 401 allocates length bytes (length, file name, line 0x284, 0); the buffer and length are stored in 0x11BF44 and 0x11BF48 and Storm ordinal 491 copies the source (buffer, source, length); its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11bf44;
__attribute__((weak)) uint32_t g_d2client_11bf48;
namespace d2cl {
uint32_t f87080(GE& e, uint32_t src, uint32_t n)
{
    if (g_d2client_11bf44) e.h(0xBE6C, 0, 0, 0, 0, 0, g_d2client_11bf44, 0xB20D4938u, 0x283, 0);
    const uint32_t buf = e.h(0xBE66, 0, 0, 0, 0, 0, n, 0xB20D4938u, 0x284, 0);
    g_d2client_11bf44 = buf;
    g_d2client_11bf48 = n;
    return e.h(0xBEE4, 0, 0, 0, 0, 0, buf, src, n);
}
}
