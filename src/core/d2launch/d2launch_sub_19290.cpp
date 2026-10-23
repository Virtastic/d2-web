// 1.13c D2Launch+0x19290 BNet login screen setup (partial: resource loads and control wiring in retail order)
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*); WK uint32_t Ordinal_10265(uint32_t); WK void Ordinal_10024(const void*, uint32_t);
WK void _exit(int);
WK void FUN_6fa5b310();
}
static void d2l_fail() { uint32_t e = Ordinal_10265(0x624); Ordinal_10024("", e); _exit(-1); }
static uint32_t d2l_slot(const void* name) { uint32_t h = Ordinal_10098(name); DAT_6fa652b8[DAT_6fa65e30++] = h; return h; }

extern "C" {
WK extern int DAT_6fa656c4, DAT_6fa65ea4, DAT_6fa65eb0, DAT_6fa65abc, DAT_6fa65e6c;
WK extern uint32_t DAT_6fa654fc, DAT_6fa6529c, DAT_6fa65518, DAT_6fa65504, DAT_6fa65e40;
WK void Ordinal_10018(void); WK int32_t Ordinal_10003(); WK intptr_t Ordinal_10020(intptr_t); WK void Ordinal_10000(void); WK void Ordinal_10202(void); WK void Ordinal_10010(void); WK void Ordinal_10170(void);
WK void Ordinal_10185(void); WK void Ordinal_10053(int32_t); WK void Ordinal_10146(void); WK int32_t Ordinal_10227(void); WK void Ordinal_10035(void*); WK void* Ordinal_10045(const char*, int, int); WK int Ordinal_422(...);
WK void FUN_6fa57df0(); WK uint32_t FUN_6fa4ee10(); WK void FUN_6fa52b10(); WK void FUN_6fa522c0();
WK void OutputDebugStringA(const char*);
WK extern char DAT_6fa61310[], DAT_6fa61340[], DAT_6fa61400[], DAT_6fa61430[], DAT_6fa61520[], DAT_6fa61580[], DAT_6fa615b0[], DAT_6fa615e0[], DAT_6fa61610[], DAT_6fa61640[], DAT_6fa61670[];
}
#define NEXT(n) do { if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; } } while (0); d2l_slot(n)
extern "C" void FUN_6fa59290() {
    DAT_6fa65e40 = 1;
    int (*conn)() = *(int (**)())(DAT_6fa656c4 + 0x68);
    if (conn()) {
        if (DAT_6fa65ea4 == 0) goto build;
        if (DAT_6fa656c4) {
            DAT_6fa65eb0 = 1;
            *(uint8_t*)(DAT_6fa65abc + 0x35d) = 0; *(uint8_t*)(DAT_6fa65abc + 0x37d) = 0;
            OutputDebugStringA("Disconnect from BNET\n"); Ordinal_10018();
            (*(void (**)())(DAT_6fa656c4 + 0x10))();
        }
    }
    FUN_6fa57df0();
    if (FUN_6fa4ee10() == 0) return;
build:
    DAT_6fa654fc = DAT_6fa6529c = DAT_6fa65518 = DAT_6fa65504 = 0;
    FUN_6fa5b310();
    NEXT(DAT_6fa61310); NEXT(DAT_6fa61340); NEXT(DAT_6fa61400); Ordinal_10003(); Ordinal_10020(0);
    NEXT(DAT_6fa61430); Ordinal_10003(); Ordinal_10020(0);
    if (Ordinal_10227() == 0) { FUN_6fa52b10(); Ordinal_10003(); Ordinal_10000(); }
    NEXT(DAT_6fa61520); Ordinal_10003(); Ordinal_10000();
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    DAT_6fa654fc = d2l_slot(DAT_6fa61580); Ordinal_10202(); Ordinal_10010(); Ordinal_10170();
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    DAT_6fa6529c = d2l_slot(DAT_6fa615b0); Ordinal_10202(); Ordinal_10010(); Ordinal_10170();
    char last[0x400]; uint32_t sel = DAT_6fa654fc;
    if (Ordinal_422("Diablo II", "Last BNet", 0, last, 0x400) != 0) { Ordinal_10185(); sel = DAT_6fa6529c; }
    Ordinal_10053(sel); Ordinal_10146(); Ordinal_10146();
    NEXT(DAT_6fa615e0); NEXT(DAT_6fa61610);
    if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return; }
    DAT_6fa65504 = d2l_slot(DAT_6fa61640);
    NEXT(DAT_6fa61670);
    FUN_6fa522c0();
    if (DAT_6fa65e6c) { FUN_6fa52b10(); Ordinal_10003(); Ordinal_10020(0); }
    Ordinal_10035(nullptr);  // link fix: ordinal 10035 is declared void elsewhere; result unusable
}
