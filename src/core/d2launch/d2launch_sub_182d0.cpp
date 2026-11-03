// 1.13c D2Launch+0x182D0 progress-bar screen (core d2l_182d0_core in d2launch_182d0.cpp)
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*) __asm__("d2launch_fc_Ordinal_10098"); WK uint32_t Ordinal_10265(uint32_t) __asm__("d2launch_fc_Ordinal_10265"); WK void Ordinal_10024(const void*, uint32_t) __asm__("d2launch_fc_Ordinal_10024"); WK void _exit(int);
WK extern char DAT_6fa64340[], DAT_6fa64370[], DAT_6fa642b0[], DAT_6fa642e0[], DAT_6fa64280[], DAT_6fa643a0[], DAT_6fa5e220[];
WK extern int DAT_6fa656c4; WK extern uint32_t DAT_6fa65e68, DAT_6fa65e64;
WK void FUN_6fa5b310(); WK int _sprintf(char*, const char*, ...);
WK void Ordinal_10174(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10174"); WK uint32_t Ordinal_10003(uint32_t) __asm__("d2launch_fc_Ordinal_10003"); WK void Ordinal_10000(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10000");
WK int32_t Ordinal_10112(uint32_t) __asm__("d2launch_fc_Ordinal_10112"); WK void Ordinal_10161(uint32_t, int32_t) __asm__("d2launch_fc_Ordinal_10161"); WK void Ordinal_10022(uint32_t, const char*, uint32_t) __asm__("d2launch_ord_10022_v");  // distinct import (other files declare 1 arg)
}
struct D2l182d0Globals { int32_t* count; uint32_t *g25e68, *g25e64; uint32_t* table; };
struct D2l182d0Seams { void (*f1b310)(); uint32_t (*win10098)(uint32_t); void (*win10174)(uint32_t, uint32_t); int32_t (*vt4c)(); uint32_t (*lang10003)(uint32_t);
                       void (*win10000)(uint32_t, uint32_t); int32_t (*win10112)(uint32_t); void (*win10161)(uint32_t, int32_t); void (*sprintf3)(char*, uint32_t, int32_t);
                       void (*win10022)(uint32_t, const char*); void (*fatal)(uint32_t); };
extern "C" void d2l_182d0_core(const D2l182d0Globals*, const uint32_t[6], uint32_t, const D2l182d0Seams*);
static uint32_t d2l_win(uint32_t t) { return (uint32_t)Ordinal_10098((const void*)(uintptr_t)t); }
static int32_t d2l_vt4c() { return (*(int32_t (**)())((uintptr_t)DAT_6fa656c4 + 0x4c))(); }
static void d2l_10000(uint32_t h, uint32_t t) { Ordinal_10000(h, t, 0, 0, 0); }
static void d2l_sp(char* b, uint32_t f, int32_t p) { _sprintf(b, (const char*)(uintptr_t)f, p); }
static void d2l_10022(uint32_t h, const char* b) { Ordinal_10022(h, b, 0); }
static void d2l_fail(uint32_t id) { uint32_t e = Ordinal_10265(id); Ordinal_10024("", e); _exit(-1); }
// 1.13c D2Launch+0x182D0
extern "C" void FUN_6fa582d0() {
    static const D2l182d0Seams S = { FUN_6fa5b310, d2l_win, Ordinal_10174, d2l_vt4c, Ordinal_10003, d2l_10000, Ordinal_10112, Ordinal_10161, d2l_sp, d2l_10022, d2l_fail };
    const D2l182d0Globals g = { &DAT_6fa65e30, &DAT_6fa65e68, &DAT_6fa65e64, DAT_6fa652b8 };
    const char* const n[6] = { DAT_6fa64340, DAT_6fa64370, DAT_6fa642b0, DAT_6fa642e0, DAT_6fa64280, DAT_6fa643a0 };
    uint32_t t[6]; for (int i = 0; i < 6; i++) t[i] = (uint32_t)(uintptr_t)n[i];
    d2l_182d0_core(&g, t, (uint32_t)(uintptr_t)DAT_6fa5e220, &S);
}
