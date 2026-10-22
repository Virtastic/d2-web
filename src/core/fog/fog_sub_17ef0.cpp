// Fog ordinal 10101 (Fog+0x17EF0, __fastcall(ecx=a, edx=b)): Storm init. Calls Storm #270(DAT 0x6FF76FE8 string),
// #263(a), #274(0,0), #286(b), stores a to Fog global 0x2DAAC, then #304 x3 with (2,&0x8000,4), (1,&1,4), (3,&4,4).
#include <cstdint>
typedef void (*storm_fn)(uint32_t, uint32_t, uint32_t);
extern "C" uint32_t g_fog_2daac;
// 1.13c Fog+0x17EF0
extern "C" void fog_sub_17ef0(uint32_t a, uint32_t b, uint32_t str, storm_fn s270, storm_fn s263, storm_fn s274,
                              storm_fn s286, storm_fn s304) {
    uint32_t v;
    s270(str, 0, 0);
    s263(a, 0, 0);
    s274(0, 0, 0);
    s286(b, 0, 0);
    g_fog_2daac = a;
    v = 0x8000; s304(2, (uint32_t)(uintptr_t)&v, 4);
    v = 1;      s304(1, (uint32_t)(uintptr_t)&v, 4);
    v = 4;      s304(3, (uint32_t)(uintptr_t)&v, 4);
}
