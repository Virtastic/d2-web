// 1.13c D2Client+0x4A2F0 -- No arguments. When 0x46150 answers 0, 0x48150 runs and 0 is returned. Otherwise a set dword 0x119763 runs 0xA3D20 and is cleared, the dword 0x119663 becomes 1, the pointer 0x119675 becomes the 0x16-byte record of the table 0xF37A8 that 0x45C00 indexes, 0x49D80 runs with one stack word 0xD35 and 1 is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint8_t g_d2client_119661[0x118];   // the packed UI block 0x119660..0x119778
static inline uint32_t u32(uint32_t rva) { uint32_t v; __builtin_memcpy(&v, g_d2client_119661 + (rva - 0x119661), 4); return v; }
static inline void w32(uint32_t rva, uint32_t v) { __builtin_memcpy(g_d2client_119661 + (rva - 0x119661), &v, 4); }
static inline uint32_t u16(uint32_t rva) { uint16_t v; __builtin_memcpy(&v, g_d2client_119661 + (rva - 0x119661), 2); return v; }
static inline void w16(uint32_t rva, uint32_t v) { const uint16_t x = (uint16_t)v; __builtin_memcpy(g_d2client_119661 + (rva - 0x119661), &x, 2); }
static inline uint32_t u8(uint32_t rva) { return g_d2client_119661[rva - 0x119661]; }
static inline void w8(uint32_t rva, uint32_t v) { g_d2client_119661[rva - 0x119661] = (uint8_t)v; }
namespace d2cl {
uint32_t f4a2f0(GE& e)
{
    if (!e.h(0x46150)) {
        e.h(0x48150);
        return 0;
    }
    if (u32(0x119763)) {
        e.h(0xA3D20);
        w32(0x119763, 0);
    }
    w32(0x119663, 1);
    w32(0x119675, 0xB20F37A8u + e.h(0x45C00) * 0x16);
    e.h(0x49D80, 0, 0, 0, 0, 0, 0xd35);
    return 1;
}
}
