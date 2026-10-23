// 1.13c D2Launch+0x182D0 progress-bar screen: percent from vtable+0x4c, clamped to 100
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*); WK uint32_t Ordinal_10265(uint32_t); WK void Ordinal_10024(const void*, uint32_t);
WK void FUN_6fa55d30(); WK void FUN_6fa52bf0(const char*); WK void _exit(int);
WK extern char DAT_6fa61040[], DAT_6fa61070[], DAT_6fa610a0[];

WK extern int DAT_6fa656c4; WK extern uint32_t DAT_6fa65e68, DAT_6fa65e64;
WK void FUN_6fa5b310(); WK int _sprintf(char*, const char*, ...);
WK intptr_t Ordinal_10174(intptr_t,intptr_t,intptr_t); WK int32_t Ordinal_10003(); WK void Ordinal_10000(void); WK int Ordinal_10112(...);
WK void Ordinal_10161(...); WK void Ordinal_10022(int32_t);
}
static void d2l_fail() { uint32_t e = Ordinal_10265(0x624); Ordinal_10024("", e); _exit(-1); }
static uint32_t d2l_slot(const void* name) { uint32_t h = Ordinal_10098(name); DAT_6fa652b8[DAT_6fa65e30++] = h; return h; }

extern "C" void FUN_6fa582d0() {
    FUN_6fa5b310();
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    d2l_slot(nullptr);
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    DAT_6fa65e68 = d2l_slot(nullptr);
    Ordinal_10174(0,0,0);
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    DAT_6fa65e64 = d2l_slot(nullptr);
    int pct = (*(int (**)())(DAT_6fa656c4 + 0x4c))();
    if (pct > 100) pct = 100;
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    d2l_slot(nullptr); Ordinal_10003(); Ordinal_10000();
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    d2l_slot(nullptr); Ordinal_10003(); Ordinal_10000();
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    d2l_slot(nullptr);
    int w = Ordinal_10112(DAT_6fa65e68);
    Ordinal_10161(DAT_6fa65e68, (w * pct) / 100);
    char buf[64]; _sprintf(buf, "%d%%", pct);
    Ordinal_10022(0);
}
