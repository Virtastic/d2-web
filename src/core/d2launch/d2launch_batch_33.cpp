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

// 1.13c D2Launch+0x15D30
void d2l_15d30_FUN_6fa55d30(void) {
  for (int32_t i = 0; i < DAT_6fa65e30; i++) Ordinal_10050(DAT_6fa652b8 + i * 4);
  DAT_6fa65e38 = 0; DAT_6fa65e3c = 0; DAT_6fa65e30 = 0;
  d2l_12b60_FUN_6fa52b60();
  Ordinal_10006(0);
  if (DAT_6fa65e5c) { FUN_6fa52bf0("palette\\fechar\\pal.pl2"); DAT_6fa65e5c = 0; }
  FUN_6fa4ae60(); FUN_6fa4bdf0(); Ordinal_10149(1);
}
