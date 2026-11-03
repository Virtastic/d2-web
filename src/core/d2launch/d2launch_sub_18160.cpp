// 1.13c D2Launch+0x18160 title/legal screen background (core d2l_18160_core in d2launch_18160.cpp)
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*) __asm__("d2launch_fc_Ordinal_10098"); WK uint32_t Ordinal_10265(uint32_t) __asm__("d2launch_fc_Ordinal_10265"); WK void Ordinal_10024(const void*, uint32_t) __asm__("d2launch_fc_Ordinal_10024");
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void _exit(int);
WK extern char DAT_6fa60f80[], DAT_6fa60fb0[], DAT_6fa60fe0[], DAT_6fa61040[], DAT_6fa61070[], DAT_6fa5e988[], DAT_6fa5e99c[];
WK int32_t Ordinal_10227() __asm__("d2launch_fc_Ordinal_10227"); WK void Ordinal_10149(...) __asm__("d2launch_fc_Ordinal_10149");
WK uint32_t Ordinal_10003(uint32_t) __asm__("d2launch_fc_Ordinal_10003"); WK void Ordinal_10000(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10000");
WK int32_t Ordinal_10035() __asm__("d2launch_fc_Ordinal_10035"); WK void Ordinal_10045(uint32_t) __asm__("d2launch_fc_Ordinal_10045");
}
struct D2l18160Seams { void (*f15d30)(); uint32_t (*win10098)(uint32_t); void (*f12bf0)(uint32_t, uint32_t); void (*fatal)(uint32_t); int (*fog10227)();
                       uint32_t (*lang10003)(uint32_t); void (*win10000)(uint32_t, uint32_t); void (*win10149)(uint32_t); int (*snd10035)(); void (*snd10045)(uint32_t); };
extern "C" void d2l_18160_core(int32_t*, uint32_t*, const uint32_t[5], uint32_t, uint32_t, const D2l18160Seams*);
static uint32_t d2l_win(uint32_t t) { return (uint32_t)Ordinal_10098((const void*)(uintptr_t)t); }
static void d2l_pal(uint32_t, uint32_t name) { FUN_6fa52bf0((const char*)(uintptr_t)name); }  // EAX = DAT_6fa5e99c (hidden register argument) rides in the seam
static void d2l_fail(uint32_t id) { uint32_t e = Ordinal_10265(id); Ordinal_10024("", e); _exit(-1); }
static void d2l_10000(uint32_t h, uint32_t t) { Ordinal_10000(h, t, 4, 0, 0); }
static void d2l_149(uint32_t z) { Ordinal_10149((int32_t)z); }
// 1.13c D2Launch+0x18160
extern "C" void FUN_6fa58160() {
    static const D2l18160Seams S = { FUN_6fa55d30, d2l_win, d2l_pal, d2l_fail, Ordinal_10227, Ordinal_10003, d2l_10000, d2l_149, Ordinal_10035, Ordinal_10045 };
    const uint32_t t[5] = { (uint32_t)(uintptr_t)DAT_6fa60f80, (uint32_t)(uintptr_t)DAT_6fa60fb0, (uint32_t)(uintptr_t)DAT_6fa61040,
                            (uint32_t)(uintptr_t)DAT_6fa61070, (uint32_t)(uintptr_t)DAT_6fa60fe0 };
    d2l_18160_core(&DAT_6fa65e30, DAT_6fa652b8, t, (uint32_t)(uintptr_t)DAT_6fa5e988, (uint32_t)(uintptr_t)DAT_6fa5e99c, &S);
}
