// 1.13c D2Launch+0x18900 realm-select screen setup (core d2l_18900_core in d2launch_18900.cpp)
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*) __asm__("d2launch_fc_Ordinal_10098"); WK uint32_t Ordinal_10265(uint32_t) __asm__("d2launch_fc_Ordinal_10265"); WK void Ordinal_10024(const void*, uint32_t) __asm__("d2launch_fc_Ordinal_10024"); WK void _exit(int);
WK void FUN_6fa5b310(); WK void FUN_6fa567b0(uint32_t);
WK void Ordinal_10023(uint32_t) __asm__("d2launch_fc_Ordinal_10023"); WK void Ordinal_10014() __asm__("d2launch_fc_Ordinal_10014"); WK void Ordinal_10061(uint32_t) __asm__("d2launch_fc_Ordinal_10061");
WK uint32_t Ordinal_10003(uint32_t) __asm__("d2launch_fc_Ordinal_10003"); WK void d2l_win10020_fc(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10020"); WK void Ordinal_10147(uint32_t, uint32_t) __asm__("d2launch_fc_Ordinal_10147");
WK int Ordinal_422(...) __asm__("d2launch_fc_Ordinal_422"); WK int Ordinal_425(...) __asm__("d2launch_fc_Ordinal_425"); WK int32_t Ordinal_10227(void) __asm__("d2launch_fc_Ordinal_10227");
WK int _sprintf(char*, const char*, ...); WK int _sscanf(const char*, const char*, ...);
WK extern char DAT_6fa61880[], DAT_6fa618b0[], DAT_6fa618e0[], DAT_6fa61910[], DAT_6fa61940[], DAT_6fa61970[], DAT_6fa619a0[], DAT_6fa619d0[];
WK extern char DAT_6fa5e204[], DAT_6fa5e210[], DAT_6fa5e1f8[];
}
struct D2l18900Globals { int32_t* count; uint32_t* table; };
struct D2l18900Seams { void (*snd10023)(uint32_t); void (*snd10014)(); void (*snd10061)(uint32_t); void (*f1b310)(); int32_t (*storm422)(uint32_t, uint32_t, char*);
                       void (*scan)(const char*, uint32_t, uint32_t[4]); void (*fmtDefault)(char*, uint32_t); void (*storm425)(uint32_t, uint32_t, const char*);
                       int32_t (*fog10227)(); void (*f167b0)(uint32_t); uint32_t (*win10098)(uint32_t); uint32_t (*lang10003)(uint32_t);
                       void (*win10020)(uint32_t, uint32_t); void (*win10147)(uint32_t, uint32_t); void (*fatal)(uint32_t); };
extern "C" void d2l_18900_core(const D2l18900Globals*, const uint32_t[8], uint32_t, uint32_t, uint32_t, uint32_t, const D2l18900Seams*);
static int32_t d2l_422(uint32_t a, uint32_t k, char* b) { return Ordinal_422((const char*)(uintptr_t)a, (const char*)(uintptr_t)k, 0, b, 0x64); }
static void d2l_scan(const char* b, uint32_t f, uint32_t o[4]) { _sscanf(b, (const char*)(uintptr_t)f, &o[0], &o[1], &o[2], &o[3]); }
static void d2l_def(char* b, uint32_t f) { _sprintf(b, (const char*)(uintptr_t)f, 0xd8, 0x94, 0xf6, 0x22); }
static void d2l_425(uint32_t a, uint32_t k, const char* b) { Ordinal_425((const char*)(uintptr_t)a, (const char*)(uintptr_t)k, 0, b); }
static uint32_t d2l_win(uint32_t t) { return (uint32_t)Ordinal_10098((const void*)(uintptr_t)t); }
static void d2l_fail(uint32_t id) { uint32_t e = Ordinal_10265(id); Ordinal_10024("", e); _exit(-1); }
// 1.13c D2Launch+0x18900
extern "C" uint32_t FUN_6fa58900() {
    static const D2l18900Seams S = { Ordinal_10023, Ordinal_10014, Ordinal_10061, FUN_6fa5b310, d2l_422, d2l_scan, d2l_def, d2l_425, Ordinal_10227, FUN_6fa567b0,
                                     d2l_win, Ordinal_10003, d2l_win10020_fc, Ordinal_10147, d2l_fail };
    const D2l18900Globals g = { &DAT_6fa65e30, DAT_6fa652b8 };
    const char* const n[8] = { DAT_6fa61880, DAT_6fa618b0, DAT_6fa618e0, DAT_6fa61910, DAT_6fa61940, DAT_6fa61970, DAT_6fa619a0, DAT_6fa619d0 };
    uint32_t t[8]; for (int i = 0; i < 8; i++) t[i] = (uint32_t)(uintptr_t)n[i];
    d2l_18900_core(&g, t, (uint32_t)(uintptr_t)DAT_6fa5e204, (uint32_t)(uintptr_t)DAT_6fa5e210, (uint32_t)(uintptr_t)DAT_6fa5e1f8, (uint32_t)(uintptr_t)DAT_6fa5e1f8, &S);
    return 1;
}
