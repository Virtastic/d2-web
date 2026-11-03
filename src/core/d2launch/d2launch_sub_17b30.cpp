// 1.13c D2Launch+0x17B30 disconnect from BNET, rebuild the screen resource list
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*) __asm__("d2launch_fc_Ordinal_10098"); WK uint32_t Ordinal_10265(uint32_t) __asm__("d2launch_fc_Ordinal_10265"); WK void Ordinal_10024(const void*, uint32_t) __asm__("d2launch_fc_Ordinal_10024");
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void _exit(int);
WK extern char DAT_6fa61040[], DAT_6fa61070[], DAT_6fa610a0[];

WK extern int DAT_6fa656c4, DAT_6fa65abc, DAT_6fa65e40, DAT_6fa65eb0;
WK extern uint32_t DAT_6fa652a0, DAT_6fa65514, DAT_6fa65510, DAT_6fa65ac8, DAT_6fa65acc, DAT_6fa652ac, DAT_6fa654f8;
WK extern char DAT_6fa63f80[], DAT_6fa63fb0[], DAT_6fa63fe0[], DAT_6fa64040[], DAT_6fa64070[], DAT_6fa640a0[], DAT_6fa640d0[], DAT_6fa64100[], DAT_6fa64130[], DAT_6fa64010[];
WK void OutputDebugStringA(const char*);
WK void Ordinal_10018() __asm__("d2launch_fc_Ordinal_10018"); WK int32_t Ordinal_10003(uint32_t) __asm__("d2launch_fc_Ordinal_10003"); WK void d2l_win10020_fc(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10020"); WK void Ordinal_10131(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10131");
WK void Ordinal_10031(int32_t) __asm__("d2launch_fc_Ordinal_10031"); WK int Ordinal_10069(uint32_t v) __asm__("d2launch_fc_Ordinal_10069"); WK void Ordinal_10201(uint32_t) __asm__("d2launch_fc_Ordinal_10201"); WK void FUN_6fa52560();
WK extern char DAT_6fa5e988[], DAT_6fa5e99c[];
}
struct D2l17b30Globals { int32_t *g25e40, *g25eb0, *count; uint32_t *g252a0, *g25514, *g25510, *g25ac8, *g25acc, *g252ac, *g254f8; uint32_t* table; };
struct D2l17b30Seams { void (*dbgStr)(uint32_t); void (*mcp10018)(); void (*vt10)(uint32_t); void (*f15d30)(); void (*f12bf0)(uint32_t, uint32_t); uint32_t (*win10098)(uint32_t);
                       uint32_t (*lang10003)(uint32_t); void (*win10020)(uint32_t, uint32_t); void (*win10131)(uint32_t, uint32_t); void (*win10031)(uint32_t);
                       void (*win10069)(uint32_t); void (*win10201)(uint32_t); void (*f12560)(); void (*fatal)(uint32_t); };
extern "C" void d2l_17b30_core(const D2l17b30Globals*, uint32_t, uint8_t*, const uint32_t[10], uint32_t, uint32_t, uint32_t, const D2l17b30Seams*);
static void d2l_dbg(uint32_t p) { OutputDebugStringA((const char*)(uintptr_t)p); }
static void d2l_vt10(uint32_t vt) { (void)vt; (*(void (**)())(DAT_6fa656c4 + 0x10))(); }
static uint32_t d2l_win(uint32_t t) { return (uint32_t)Ordinal_10098((const void*)(uintptr_t)t); }
static void d2l_pal(uint32_t, uint32_t name) { FUN_6fa52bf0((const char*)(uintptr_t)name); }  // EAX = DAT_6fa5e99c (hidden register argument) rides in the seam
static uint32_t d2l_str(uint32_t id) { return (uint32_t)Ordinal_10003(id); }
static void d2l_10020(uint32_t h, uint32_t t) { d2l_win10020_fc(h, t); }
static void d2l_10131(uint32_t h, uint32_t z) { Ordinal_10131(h, z); }
static void d2l_10031(uint32_t h) { Ordinal_10031((int32_t)h); }
static void d2l_10069(uint32_t h) { Ordinal_10069(h); }
static void d2l_10201(uint32_t h) { Ordinal_10201(h); }
static void d2l_fail(uint32_t id) { uint32_t e = Ordinal_10265(id); Ordinal_10024("", e); _exit(-1); }
// 1.13c D2Launch+0x17B30
extern "C" void FUN_6fa57b30() {
    static const D2l17b30Seams S = { d2l_dbg, Ordinal_10018, d2l_vt10, FUN_6fa55d30, d2l_pal, d2l_win, d2l_str, d2l_10020, d2l_10131, d2l_10031, d2l_10069, d2l_10201, FUN_6fa52560, d2l_fail };
    const D2l17b30Globals g = { &DAT_6fa65e40, &DAT_6fa65eb0, &DAT_6fa65e30, &DAT_6fa652a0, &DAT_6fa65514, &DAT_6fa65510, &DAT_6fa65ac8, &DAT_6fa65acc, &DAT_6fa652ac, &DAT_6fa654f8, DAT_6fa652b8 };
    const char* const n[10] = { DAT_6fa63f80, DAT_6fa63fb0, DAT_6fa63fe0, DAT_6fa64040, DAT_6fa64070, DAT_6fa640a0, DAT_6fa640d0, DAT_6fa64100, DAT_6fa64130, DAT_6fa64010 };
    uint32_t t[10]; for (int i = 0; i < 10; i++) t[i] = (uint32_t)(uintptr_t)n[i];
    d2l_17b30_core(&g, (uint32_t)DAT_6fa656c4, (uint8_t*)(uintptr_t)DAT_6fa65abc, t, (uint32_t)(uintptr_t)"Disconnect from BNET\n", (uint32_t)(uintptr_t)DAT_6fa5e988, (uint32_t)(uintptr_t)DAT_6fa5e99c, &S);
}
