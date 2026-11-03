// D2Launch batch 33: 12B60 15D30.
#include <cstdint>
__attribute__((weak)) extern void Ordinal_10050(void* p);
__attribute__((weak)) extern void Ordinal_10006(int32_t);
__attribute__((weak)) extern void Ordinal_10149(int32_t);
__attribute__((weak)) extern void FUN_6fa52bf0(const char*);
__attribute__((weak)) extern void FUN_6fa4ae60(void);
__attribute__((weak)) extern void FUN_6fa4bdf0(void);
__attribute__((weak)) extern int32_t DAT_6fa65e30, DAT_6fa65e34, DAT_6fa65e38, DAT_6fa65e3c, DAT_6fa65e5c;
__attribute__((weak)) extern uint8_t DAT_6fa652b8[];
__attribute__((weak)) extern void* DAT_6fa653c0[];

// 1.13c D2Launch+0x12B60
void d2l_12b60_FUN_6fa52b60(void) {
  for (int32_t i = 0; i < DAT_6fa65e34; i++) Ordinal_10050(&DAT_6fa653c0[i]);
  DAT_6fa65e34 = 0;
}

// 1.13c D2Launch+0x15D30 (core d2l_15d30_core in d2launch_15d30.cpp)
struct D2l15d30Seams { void (*win10050)(uint32_t); void (*win10006)(uint32_t); void (*f12bf0)(uint32_t, uint32_t); void (*fae60)(); void (*fbdf0)(); void (*win10149)(uint32_t); };
extern "C" void d2l_15d30_core(int32_t*, int32_t*, int32_t*, int32_t*, int32_t*, uint32_t, uint32_t, uint32_t, uint32_t, const D2l15d30Seams*);
__attribute__((weak)) extern char DAT_6fa5e9b0[], DAT_6fa5e9c8[];
static void d2l_a50(uint32_t p) { Ordinal_10050((void*)(uintptr_t)p); }
static void d2l_a6(uint32_t z) { Ordinal_10006((int32_t)z); }
static void d2l_apal(uint32_t, uint32_t n) { FUN_6fa52bf0((const char*)(uintptr_t)n); }  // EAX = DAT_6fa5e9c8 (hidden register argument) rides in the seam
static void d2l_a149(uint32_t v) { Ordinal_10149((int32_t)v); }
void d2l_15d30_FUN_6fa55d30(void) {
  static const D2l15d30Seams S = { d2l_a50, d2l_a6, d2l_apal, FUN_6fa4ae60, FUN_6fa4bdf0, d2l_a149 };
  d2l_15d30_core(&DAT_6fa65e30, &DAT_6fa65e34, &DAT_6fa65e38, &DAT_6fa65e3c, &DAT_6fa65e5c, (uint32_t)(uintptr_t)DAT_6fa652b8, (uint32_t)(uintptr_t)DAT_6fa653c0,
                 (uint32_t)(uintptr_t)DAT_6fa5e9b0, (uint32_t)(uintptr_t)DAT_6fa5e9c8, &S);
}
