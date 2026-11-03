// 1.13c D2Launch+0x19290 BNet login screen (core d2l_19290_core in d2launch_19290.cpp)
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*) __asm__("d2launch_fc_Ordinal_10098"); WK uint32_t Ordinal_10265(uint32_t) __asm__("d2launch_fc_Ordinal_10265"); WK void Ordinal_10024(const void*, uint32_t) __asm__("d2launch_fc_Ordinal_10024"); WK void _exit(int);
WK extern int DAT_6fa656c4, DAT_6fa65ea4, DAT_6fa65eb0, DAT_6fa65e6c, DAT_6fa65e40;
WK extern uint32_t DAT_6fa65abc, DAT_6fa654fc, DAT_6fa6529c, DAT_6fa65518, DAT_6fa65504;
WK void Ordinal_10018(void) __asm__("d2launch_fc_Ordinal_10018"); WK uint32_t Ordinal_10003(uint32_t) __asm__("d2launch_fc_Ordinal_10003"); WK void d2l_win10020_fc(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10020");
WK void Ordinal_10000(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10000");
WK void Ordinal_10202(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10202"); WK void Ordinal_10010(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10010"); WK void Ordinal_10170(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10170");
WK void Ordinal_10185(uint32_t, const char*) __asm__("d2launch_fc_Ordinal_10185"); WK void Ordinal_10053(uint32_t) __asm__("d2launch_fc_Ordinal_10053"); WK void Ordinal_10146(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10146");
WK int32_t Ordinal_10227(void) __asm__("d2launch_fc_Ordinal_10227"); WK int32_t Ordinal_10035(void) __asm__("d2launch_fc_Ordinal_10035"); WK void Ordinal_10045(uint32_t) __asm__("d2launch_fc_Ordinal_10045"); WK int Ordinal_422(...) __asm__("d2launch_fc_Ordinal_422");
WK void FUN_6fa57df0(); WK int32_t FUN_6fa4ee10(); WK uint32_t FUN_6fa52b10(uint32_t); WK void FUN_6fa522c0(); WK void FUN_6fa5b310();
WK void FUN_6fa52330(); WK void FUN_6fa526c0(); WK void FUN_6fa526b0();
WK void OutputDebugStringA(const char*);
WK extern char DAT_6fa61310[], DAT_6fa61340[], DAT_6fa61400[], DAT_6fa61430[], DAT_6fa61520[], DAT_6fa61580[], DAT_6fa615b0[], DAT_6fa615e0[], DAT_6fa61610[], DAT_6fa61640[], DAT_6fa61670[];
WK extern char DAT_6fa5e204[], DAT_6fa5e0b4[];
}
struct D2l19290Globals { int32_t *g25e40, *g25eb0, *g25ea4, *count, *g25e6c; uint32_t *g254fc, *g2529c, *g25518, *g25504; uint32_t* table; };
struct D2l19290Seams { int32_t (*vt68)(); void (*dbgStr)(uint32_t); void (*mcp10018)(); void (*vt10)(uint32_t); void (*f17df0)(); int32_t (*fee10)(); void (*f1b310)();
                       uint32_t (*win10098)(uint32_t); uint32_t (*lang10003)(uint32_t); void (*win10020)(uint32_t, uint32_t); uint32_t (*f12b10)(uint32_t); int32_t (*fog10227)();
                       void (*win10000)(uint32_t, uint32_t); void (*win10202)(uint32_t, uint32_t); void (*win10010)(uint32_t, uint32_t); void (*win10170)(uint32_t, uint32_t);
                       int32_t (*storm422)(uint32_t, uint32_t, char*); void (*win10185)(uint32_t, const char*); void (*win10053)(uint32_t); void (*win10146)(uint32_t, uint32_t);
                       void (*f122c0)(); int32_t (*snd10035)(); void (*snd10045)(uint32_t); void (*fatal)(uint32_t); };
extern "C" void d2l_19290_core(const D2l19290Globals*, uint32_t, uint8_t*, const uint32_t[11], uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const D2l19290Seams*);
static int32_t d2l_vt68() { return (*(int32_t (**)())((uintptr_t)DAT_6fa656c4 + 0x68))(); }
static void d2l_dbg(uint32_t p) { OutputDebugStringA((const char*)(uintptr_t)p); }
static void d2l_vt10(uint32_t) { (*(void (**)())((uintptr_t)DAT_6fa656c4 + 0x10))(); }
static uint32_t d2l_win(uint32_t t) { return (uint32_t)Ordinal_10098((const void*)(uintptr_t)t); }
static void d2l_10000(uint32_t h, uint32_t t) { Ordinal_10000(h, t, 0, 0, 0); }
static int32_t d2l_422(uint32_t a, uint32_t k, char* buf) { return Ordinal_422((const char*)(uintptr_t)a, (const char*)(uintptr_t)k, 0, buf, 0x400); }
static void d2l_fail(uint32_t id) { uint32_t e = Ordinal_10265(id); Ordinal_10024("", e); _exit(-1); }
// 1.13c D2Launch+0x19290
extern "C" void FUN_6fa59290() {
    static const D2l19290Seams S = { d2l_vt68, d2l_dbg, Ordinal_10018, d2l_vt10, FUN_6fa57df0, FUN_6fa4ee10, FUN_6fa5b310, d2l_win, Ordinal_10003, d2l_win10020_fc, FUN_6fa52b10,
                                     Ordinal_10227, d2l_10000, Ordinal_10202, Ordinal_10010, Ordinal_10170, d2l_422, Ordinal_10185, Ordinal_10053, Ordinal_10146, FUN_6fa522c0,
                                     Ordinal_10035, Ordinal_10045, d2l_fail };
    const D2l19290Globals g = { &DAT_6fa65e40, &DAT_6fa65eb0, &DAT_6fa65ea4, &DAT_6fa65e30, &DAT_6fa65e6c, &DAT_6fa654fc, &DAT_6fa6529c, &DAT_6fa65518, &DAT_6fa65504, DAT_6fa652b8 };
    const char* const n[11] = { DAT_6fa61310, DAT_6fa61340, DAT_6fa61400, DAT_6fa61430, DAT_6fa61520, DAT_6fa61580, DAT_6fa615b0, DAT_6fa615e0, DAT_6fa61610, DAT_6fa61640, DAT_6fa61670 };
    uint32_t t[11]; for (int i = 0; i < 11; i++) t[i] = (uint32_t)(uintptr_t)n[i];
    d2l_19290_core(&g, (uint32_t)DAT_6fa656c4, (uint8_t*)(uintptr_t)DAT_6fa65abc, t, (uint32_t)(uintptr_t)"Disconnect from BNET\n",
                   (uint32_t)(uintptr_t)DAT_6fa5e204, (uint32_t)(uintptr_t)DAT_6fa5e0b4,
                   (uint32_t)(uintptr_t)FUN_6fa52330, (uint32_t)(uintptr_t)FUN_6fa526c0, (uint32_t)(uintptr_t)FUN_6fa526b0, &S);
}
