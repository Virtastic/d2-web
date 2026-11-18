// 1.13c D2Client+0x8DFB0 -- an item's personalized name (EBX = item, [esp+4] = out, [esp+8] = template; returns 1, or 0 when
// 0xC1A8(item, 0x1000000, 0x70B, file) says no): three locals zeroed (0x200, 0x200, 0x20 chars), the item's ANSI name 0xC7EA(item)
// widened (win2Unicode, 0x20 chars) into the first, then Personalize(out, that, template, 0x200, language 0xD1E6()).
#include "cl_env.h"
namespace {
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
}
extern "C" uint32_t d2c_8dfb0(uint32_t item, uint32_t out, uint32_t tpl, const cl::Env& e) {
    if (!e.H(0xc1a8, item, 0x1000000, 0x70b, 0xB20D476Cu)) return 0;
    volatile char16_t name[0x200], spare[0x200], small[0x20];
    for (int i = 0; i < 0x200; i++) name[i] = 0;
    for (int i = 0; i < 0x200; i++) spare[i] = 0;
    for (int i = 0; i < 0x20; i++) small[i] = 0;
    uint32_t s = e.H(0xc7ea, item);
    e.H(0xceac0, 0x20, 0, 0, 0, 0, 0, A(name), s);
    uint32_t lang = e.H(0xd1e6);
    e.H(0xceaa4, tpl, 0x200, lang, 0, 0, 0, out, A(name));
    return 1;
}
