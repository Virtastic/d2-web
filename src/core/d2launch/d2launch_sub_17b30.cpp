// 1.13c D2Launch+0x17B30 disconnect from BNET, rebuild the screen resource list
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*); WK uint32_t Ordinal_10265(uint32_t); WK void Ordinal_10024(const void*, uint32_t);
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void _exit(int);
WK extern char DAT_6fa61040[], DAT_6fa61070[], DAT_6fa610a0[];

WK extern int DAT_6fa656c4, DAT_6fa65abc, DAT_6fa65e40, DAT_6fa65eb0;
WK extern uint32_t DAT_6fa652a0, DAT_6fa65514, DAT_6fa65510, DAT_6fa65ac8, DAT_6fa65acc, DAT_6fa652ac, DAT_6fa654f8;
WK extern char DAT_6fa63f80[], DAT_6fa63fb0[], DAT_6fa63fe0[], DAT_6fa64040[], DAT_6fa64070[], DAT_6fa640a0[], DAT_6fa640d0[], DAT_6fa64100[], DAT_6fa64130[], DAT_6fa64010[];
WK void OutputDebugStringA(const char*);
WK void Ordinal_10018(); WK int32_t Ordinal_10003(); WK intptr_t Ordinal_10020(intptr_t); WK void Ordinal_10131(void);
WK void Ordinal_10031(int32_t); WK int Ordinal_10069(uint32_t v); WK void Ordinal_10201(void); WK void FUN_6fa52560();
}
static void d2l_fail() { uint32_t e = Ordinal_10265(0x624); Ordinal_10024("", e); _exit(-1); }
static uint32_t d2l_slot(const void* name) { uint32_t h = Ordinal_10098(name); DAT_6fa652b8[DAT_6fa65e30++] = h; return h; }

extern "C" void FUN_6fa57b30() {
    int app = DAT_6fa65abc; DAT_6fa65e40 = 1;
    if (DAT_6fa656c4) {
        DAT_6fa65eb0 = 1; *(uint8_t*)(app + 0x35d) = 0; *(uint8_t*)(app + 0x37d) = 0;
        OutputDebugStringA("Disconnect from BNET\n"); Ordinal_10018();
        (*(void (**)())(DAT_6fa656c4 + 0x10))();
    }
    FUN_6fa55d30(); FUN_6fa52bf0("palette\\sky\\pal.pl2");
    // each step: capacity check, then load+push; globals receive the handle where retail stores one
    #define STEP(nm, g, post) if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; } { uint32_t h = d2l_slot(nm); (void)h; g; post; }
    uint32_t sink;
    STEP(DAT_6fa63f80, sink = h, ;)
    STEP(DAT_6fa63fb0, sink = h, ;)
    STEP(DAT_6fa63fe0, DAT_6fa652a0 = h, Ordinal_10003(); Ordinal_10020(0))
    STEP(DAT_6fa64040, DAT_6fa65514 = h, ;)
    STEP(DAT_6fa64070, DAT_6fa65510 = h, ;)
    STEP(DAT_6fa640a0, DAT_6fa65ac8 = h, Ordinal_10003(); Ordinal_10020(0))
    STEP(DAT_6fa640d0, DAT_6fa65acc = h, Ordinal_10003(); Ordinal_10020(0); Ordinal_10131(); Ordinal_10131())
    STEP(DAT_6fa64100, sink = h, Ordinal_10031(h))
    STEP(DAT_6fa64130, DAT_6fa652ac = h, Ordinal_10069(0); Ordinal_10201())
    STEP(DAT_6fa64010, DAT_6fa654f8 = h, ;)
    (void)sink; FUN_6fa52560();
}
