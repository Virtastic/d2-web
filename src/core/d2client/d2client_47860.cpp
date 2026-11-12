// 1.13c D2Client+0x47860 -- No arguments. Runs 0x88A70 (EBX = 2, four zero stack words), asks the object at 0x11977B through its vtable slot +0x2C (ECX = the object) and stores the answer at 0x119654 and the dword 0x11971D at 0x119658. An answer of 10 or more (signed) ends it; otherwise slot +0x34 (ECX = the object, stack: the answer) is asked and an answer of 10 or more (unsigned) ends it. With a player unit at 0x11BBFC the list at 0x11C4D4 (next at +0x30) is searched for the first node of type 7 (+4) whose id (+0xC) is the player id (+0xC) and whose dword +0x20 is zero; when its dword +8 is not -1, 0x47810 runs. Otherwise, with a zero dword 0x119854, or when 0x217E0 (ECX = that dword) answers zero and the word 0xF23E8 is 0xFFFF, 0x46C00 runs; else 0x47810. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_119654;
__attribute__((weak)) uint32_t g_d2client_119658;
__attribute__((weak)) uint32_t g_d2client_11977b;
__attribute__((weak)) uint32_t g_d2client_11bbfc;
__attribute__((weak)) uint32_t g_d2client_11c4d4;
__attribute__((weak)) uint32_t g_d2client_119854;
__attribute__((weak)) uint32_t g_d2client_f23e8;
__attribute__((weak)) uint8_t g_d2client_119661[0x118];   // the packed UI block 0x119661..0x119779
static inline uint32_t u32(uint32_t rva) { uint32_t v; __builtin_memcpy(&v, g_d2client_119661 + (rva - 0x119661), 4); return v; }
static inline void w32(uint32_t rva, uint32_t v) { __builtin_memcpy(g_d2client_119661 + (rva - 0x119661), &v, 4); }
static inline uint32_t u16(uint32_t rva) { uint16_t v; __builtin_memcpy(&v, g_d2client_119661 + (rva - 0x119661), 2); return v; }
static inline void w16(uint32_t rva, uint32_t v) { const uint16_t x = (uint16_t)v; __builtin_memcpy(g_d2client_119661 + (rva - 0x119661), &x, 2); }
static inline uint32_t u8(uint32_t rva) { return g_d2client_119661[rva - 0x119661]; }
static inline void w8(uint32_t rva, uint32_t v) { g_d2client_119661[rva - 0x119661] = (uint8_t)v; }
namespace d2cl {
uint32_t f47860(GE& e)
{
    e.h(0x88A70, 0, 0, 0, 2, 0, 0, 0, 0, 0);
    const uint32_t obj = g_d2client_11977b, vt = e.rd32(obj);
    const uint32_t n = e.h(HookEnv::handlerRva(e.rd32(vt + 0x2c)), 0, obj);
    g_d2client_119654 = n;
    g_d2client_119658 = u32(0x11971d);
    if ((int32_t)n >= 10) return 0;
    if (e.h(HookEnv::handlerRva(e.rd32(vt + 0x34)), 0, obj, 0, 0, 0, n) >= 10) return 0;
    bool busy = false;
    if (const uint32_t pl = g_d2client_11bbfc) {
        const uint32_t id = e.rd32(pl + 0xc);
        for (uint32_t p = g_d2client_11c4d4; p; p = e.rd32(p + 0x30))
            if (e.rd32(p + 4) == 7 && e.rd32(p + 0xc) == id && !e.rd32(p + 0x20)) { busy = e.rd32(p + 8) != 0xffffffffu; break; }
    }
    if (!busy) {
        const uint32_t g = g_d2client_119854;
        if (!g || (!e.h(0x217E0, 0, g) && (g_d2client_f23e8 & 0xffff) == 0xffff)) { e.h(0x46C00); return 0; }
    }
    e.h(0x47810);
    return 0;
}
}
