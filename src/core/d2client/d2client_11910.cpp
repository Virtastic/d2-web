// 1.13c D2Client+0x11910 -- stdcall mouse-wheel message handler. While the dword 0x11C3A4 is set the message is marked unhandled (+0x18 = 0) and returned. Otherwise the signed word at +0xA is divided by 120 (toward zero). A non-zero quotient asks 0x3A270 (EAX = a zeroed out-dword, ECX = the dword at 10 * the index 0x11C538 in the table the pointer 0xF8B94 names, stack: the dword 0xF8BE8, then 0x103 for a positive quotient or 0x104 for a negative one); a non-zero answer runs 0x114F0 (1), otherwise a non-zero low word of the out-dword is stored in the word 0x11C54C and GetTickCount in the dword 0x11C550. The message is then marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_f8be8;
__attribute__((weak)) uint32_t g_d2client_11c538;
__attribute__((weak)) uint32_t g_d2client_11c54c;
__attribute__((weak)) uint32_t g_d2client_11c550;
static inline uint32_t m16(uint32_t a) { uint16_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
namespace d2cl {
uint32_t f11910(GE& e, uint32_t, uint32_t msg, uint32_t tb, uint32_t t)
{
    if (g_d2client_11c3a4) {
        e.wr32(msg + 0x18, 0);
        return msg;
    }
    const int32_t q = (int16_t)m16(msg + 0xa) / 120;
    if (q) {
        uint32_t out = 0;
        const uint32_t r = e.h(0x3A270, e.stk(&out), e.rd32(tb + g_d2client_11c538 * 10), 0, 0, 0, g_d2client_f8be8, (q < 0) + 0x103);
        if (r) {
            e.h(0x114F0, 0, 0, 0, 0, 0, 1);
        } else if (out & 0xffff) {
            g_d2client_11c54c = (g_d2client_11c54c & 0xffff0000u) | (out & 0xffff);
            g_d2client_11c550 = e.tick(t);
        }
    }
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
