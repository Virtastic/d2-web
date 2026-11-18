// 1.13c D2Client+0x8E080 -- personalize a name in place (EBX = item, [esp+4] = string; returns 1, or 0): 0 unless
// 0xC1A8(item, 0x1000000, 0x6EA, file) and the item quality 0xC19C(item) is outside 5..9. Three locals zeroed (0x200, 0x200, 0x20
// chars), the ANSI name 0xC7EA(item) widened (win2Unicode, 0x20 chars) into the first, Personalize(second, first, string, 0x200,
// language 0xD1E6()), then string = second.
#include "cl_env.h"
namespace {
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
}
extern "C" uint32_t d2c_8e080(uint32_t item, uint32_t str, const cl::Env& e) {
    if (!e.H(0xc1a8, item, 0x1000000, 0x6ea, 0xB20D476Cu)) return 0;
    int32_t q = (int32_t)e.H(0xc19c, item);
    if (q >= 5 && q <= 9) return 0;
    volatile char16_t name[0x200], res[0x200], small[0x20];
    for (int i = 0; i < 0x200; i++) name[i] = 0;
    for (int i = 0; i < 0x200; i++) res[i] = 0;
    for (int i = 0; i < 0x20; i++) small[i] = 0;
    uint32_t s = e.H(0xc7ea, item);
    e.H(0xceac0, 0x20, 0, 0, 0, 0, 0, A(name), s);
    uint32_t lang = e.H(0xd1e6);
    e.H(0xceaa4, str, 0x200, lang, 0, 0, 0, A(res), A(name));
    char16_t* d = (char16_t*)(uintptr_t)str;
    const volatile char16_t* p = res;
    while ((*d = *p)) { d++; p++; }
    return 1;
}
