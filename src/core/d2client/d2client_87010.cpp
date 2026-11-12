// 1.13c D2Client+0x87010 -- stdcall, two arguments (destination, length slot); the result is the low byte. Without a pending buffer (the dword 0x11BF44) the byte is 0. Otherwise the slot is clamped to the pending size 0x11BF48 (unsigned), Storm ordinal 491 copies that many bytes (destination, buffer, count), Storm ordinal 403 frees the buffer (buffer, the file name at 0xD4938, line 0x292, 0), both dwords are cleared and the byte is 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11bf44;
__attribute__((weak)) uint32_t g_d2client_11bf48;
namespace d2cl {
uint32_t f87010(GE& e, uint32_t eax, uint32_t dst, uint32_t pLen)
{
    const uint32_t buf = g_d2client_11bf44;
    if (!buf) return eax & ~0xffu;
    uint32_t n = e.rd32(pLen);
    if (n >= g_d2client_11bf48) n = g_d2client_11bf48;
    e.wr32(pLen, n);
    e.h(0xBEE4, 0, 0, 0, 0, 0, dst, buf, n);
    const uint32_t r = e.h(0xBE6C, 0, 0, 0, 0, 0, buf, 0xB20D4938u, 0x292, 0);
    g_d2client_11bf44 = 0;
    g_d2client_11bf48 = 0;
    return (r & ~0xffu) | 1;
}
}
