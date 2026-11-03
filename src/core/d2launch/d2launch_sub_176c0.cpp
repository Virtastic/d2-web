// 1.13c D2Launch+0x176C0 sky-palette screen resources (core d2l_176c0_core in d2launch_176c0.cpp)
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*) __asm__("d2launch_fc_Ordinal_10098"); WK void Ordinal_10091(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10091"); WK uint32_t Ordinal_10265(uint32_t) __asm__("d2launch_fc_Ordinal_10265"); WK void Ordinal_10024(const void*, uint32_t) __asm__("d2launch_fc_Ordinal_10024");
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void _exit(int);
WK extern char DAT_6fa61040[], DAT_6fa61070[], DAT_6fa610a0[], DAT_6fa5e988[], DAT_6fa5e99c[];
}
struct D2l176c0Seams { void (*f15d30)(); uint32_t (*win10098)(uint32_t); void (*win10091)(uint32_t, uint32_t); void (*f12bf0)(uint32_t, uint32_t); void (*fatal)(uint32_t); };
extern "C" void d2l_176c0_core(int32_t*, uint32_t*, const uint32_t[3], uint32_t, uint32_t, const D2l176c0Seams*);
static uint32_t d2l_win(uint32_t t) { return (uint32_t)Ordinal_10098((const void*)(uintptr_t)t); }
static void d2l_hide(uint32_t h, uint32_t z) { Ordinal_10091(h, z); }
static void d2l_pal(uint32_t, uint32_t name) { FUN_6fa52bf0((const char*)(uintptr_t)name); }  // EAX = DAT_6fa5e99c (hidden register argument) rides in the seam
static void d2l_fail(uint32_t id) { uint32_t e = Ordinal_10265(id); Ordinal_10024("", e); _exit(-1); }
// 1.13c D2Launch+0x176C0
extern "C" void FUN_6fa576c0() {
    static const D2l176c0Seams S = { FUN_6fa55d30, d2l_win, d2l_hide, d2l_pal, d2l_fail };
    const uint32_t t[3] = { (uint32_t)(uintptr_t)DAT_6fa61040, (uint32_t)(uintptr_t)DAT_6fa61070, (uint32_t)(uintptr_t)DAT_6fa610a0 };
    d2l_176c0_core(&DAT_6fa65e30, DAT_6fa652b8, t, (uint32_t)(uintptr_t)DAT_6fa5e988, (uint32_t)(uintptr_t)DAT_6fa5e99c, &S);
}
