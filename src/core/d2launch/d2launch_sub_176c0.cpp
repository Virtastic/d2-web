// 1.13c D2Launch+0x176C0 load three sky-palette screen resources, then palette\sky\pal.pl2
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*); WK uint32_t Ordinal_10265(uint32_t); WK void Ordinal_10024(const void*, uint32_t);
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void _exit(int);
WK extern char DAT_6fa61040[], DAT_6fa61070[], DAT_6fa610a0[];

WK void Ordinal_10091(...);
}
static void d2l_fail() { uint32_t e = Ordinal_10265(0x624); Ordinal_10024("", e); _exit(-1); }
static uint32_t d2l_slot(const void* name) { uint32_t h = Ordinal_10098(name); DAT_6fa652b8[DAT_6fa65e30++] = h; return h; }

extern "C" void FUN_6fa576c0() {
    FUN_6fa55d30();
    const void* n[3] = { DAT_6fa61040, DAT_6fa61070, DAT_6fa610a0 };
    for (int i = 0; i < 3; i++) {
        if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
        uint32_t h = d2l_slot(n[i]);
        if (i < 2) Ordinal_10091(h, 0);
    }
    FUN_6fa52bf0("palette\\sky\\pal.pl2");
}
