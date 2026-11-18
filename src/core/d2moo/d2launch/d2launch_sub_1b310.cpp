// 1.13c D2Launch+0x1B310 common background (core d2l_1b310_core in d2launch_1b310.cpp): FUN_55d30, three resource pushes, the sky palette, FUN_510c0
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*) __asm__("d2launch_fc_Ordinal_10098"); WK uint32_t Ordinal_10265(uint32_t) __asm__("d2launch_fc_Ordinal_10265"); WK void Ordinal_10024(const void*, uint32_t) __asm__("d2launch_fc_Ordinal_10024");
WK void _exit(int);
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void FUN_6fa510c0();
WK extern char DAT_6fa610a0[], DAT_6fa61040[], DAT_6fa61070[], DAT_6fa5e988[], DAT_6fa5e99c[];
}
struct D2l1b310Seams { void (*f15d30)(); uint32_t (*win10098)(uint32_t); void (*f12bf0)(uint32_t, uint32_t); void (*f110c0)(); void (*fatal)(uint32_t); };
extern "C" void d2l_1b310_core(int32_t*, uint32_t*, const uint32_t[3], uint32_t, uint32_t, const D2l1b310Seams*);
static uint32_t d2l_win(uint32_t t) { return (uint32_t)Ordinal_10098((const void*)(uintptr_t)t); }
static void d2l_pal(uint32_t, uint32_t name) { FUN_6fa52bf0((const char*)(uintptr_t)name); }  // EAX = DAT_6fa5e99c (hidden register argument) rides in the seam
static void d2l_fail(uint32_t id) { uint32_t e = Ordinal_10265(id); Ordinal_10024("", e); _exit(-1); }
extern "C" void FUN_6fa5b310() {
    static const D2l1b310Seams S = { FUN_6fa55d30, d2l_win, d2l_pal, FUN_6fa510c0, d2l_fail };
    const uint32_t t[3] = { (uint32_t)(uintptr_t)DAT_6fa610a0, (uint32_t)(uintptr_t)DAT_6fa61040, (uint32_t)(uintptr_t)DAT_6fa61070 };
    d2l_1b310_core(&DAT_6fa65e30, DAT_6fa652b8, t, (uint32_t)(uintptr_t)DAT_6fa5e988, (uint32_t)(uintptr_t)DAT_6fa5e99c, &S);
}
