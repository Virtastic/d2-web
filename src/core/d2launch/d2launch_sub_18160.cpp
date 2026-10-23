// 1.13c D2Launch+0x18160 load four screen resources + pl2 + fifth, set up widgets
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*); WK uint32_t Ordinal_10265(uint32_t); WK void Ordinal_10024(const void*, uint32_t);
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void _exit(int);
WK extern char DAT_6fa61040[], DAT_6fa61070[], DAT_6fa610a0[];

WK extern char DAT_6fa60f80[], DAT_6fa60fb0[], DAT_6fa60fe0[];
WK int32_t Ordinal_10227(); WK int32_t Ordinal_10003(); WK void Ordinal_10000(); WK void Ordinal_10149(...);
WK void Ordinal_10035(void*); WK void* Ordinal_10045(const char*, int, int);
}
static void d2l_fail() { uint32_t e = Ordinal_10265(0x624); Ordinal_10024("", e); _exit(-1); }
static uint32_t d2l_slot(const void* name) { uint32_t h = Ordinal_10098(name); DAT_6fa652b8[DAT_6fa65e30++] = h; return h; }

extern "C" void FUN_6fa58160() {
    FUN_6fa55d30();
    const void* n[5] = { DAT_6fa60f80, DAT_6fa60fb0, DAT_6fa61040, DAT_6fa61070, DAT_6fa60fe0 };
    for (int i = 0; i < 5; i++) {
        if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
        d2l_slot(n[i]);
        if (i == 3) FUN_6fa52bf0("palette\\sky\\pal.pl2");
    }
    Ordinal_10227(); Ordinal_10003(); Ordinal_10000(); Ordinal_10003(); Ordinal_10000();
    Ordinal_10149(0);
    Ordinal_10035(nullptr);  // link fix: ordinal 10035 void elsewhere
}
