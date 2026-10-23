// D2Launch batch b4f0: B4F0 B5B0 BDA0 BDF0. AF00 (CharSel list draw, 1105 B, register-passed args) is spec-only for now.
#include <cstdint>

__attribute__((weak)) extern void Ordinal_10147(void* ctl);       // D2Win destroy control
__attribute__((weak)) extern int32_t Ordinal_10227(void);
__attribute__((weak)) extern void Ordinal_10050(void* p);
__attribute__((weak)) extern void Ordinal_403(void* p, const char* file, int32_t line, int32_t);  // Fog free
__attribute__((weak)) extern void Ordinal_10024(const char* a, void* b);
__attribute__((weak)) extern void* Ordinal_10265(int32_t id);
__attribute__((weak)) extern void FUN_6fa4a8c0(void);
__attribute__((weak)) extern void FUN_6fa4b4f0(void);
__attribute__((weak)) extern void FUN_6fa52070(void);
__attribute__((weak)) extern void _exit(int);
__attribute__((weak)) extern uint32_t DAT_6fa64db0, DAT_6fa65ad4, DAT_6fa65e34, DAT_6fa65984, DAT_6fa65e78, DAT_6fa65ef4;
__attribute__((weak)) extern void* DAT_6fa653c0[];
__attribute__((weak)) extern int32_t** DAT_6fa65e74;

// 1.13c D2Launch+0xB4F0
void d2l_b4f0_FUN_6fa4b4f0(void* c0, void* c1, void* c2, void* c3, void* c4, void* c5, void* c6) {
  Ordinal_10147(c0); Ordinal_10147(c1); Ordinal_10147(c2); Ordinal_10147(c3); Ordinal_10147(c4);
  if (Ordinal_10227()) { Ordinal_10147(c5); Ordinal_10147(c6); }
  DAT_6fa64db0 = 0;
  FUN_6fa4a8c0();
}

// 1.13c D2Launch+0xB5B0  (idx in EAX)
int32_t d2l_b5b0_FUN_6fa4b5b0(int32_t idx) {
  if ((int32_t)DAT_6fa65e78 <= idx) return 0;
  int32_t* p = DAT_6fa65e74[idx];
  if (!p) { Ordinal_10024("", Ordinal_10265(0x1143)); _exit(-1); }
  return *p;
}

// 1.13c D2Launch+0xBDA0
void d2l_bda0_FUN_6fa4bda0(void) {
  if (DAT_6fa65ad4 == 1) {
    for (int32_t i = 0; i < (int32_t)DAT_6fa65e34; i++) Ordinal_10050(&DAT_6fa653c0[i]);
    DAT_6fa65e34 = 0;
    FUN_6fa4b4f0();
  }
  DAT_6fa65984 = 0;
}

// 1.13c D2Launch+0xBDF0
void d2l_bdf0_FUN_6fa4bdf0(void) {
  uint8_t* n = (uint8_t*)(uintptr_t)DAT_6fa65ef4;
  while (n) {
    uint8_t* next = *(uint8_t**)(n + 0xc);
    Ordinal_403(*(void**)(n + 4), "..\\Source\\D2Launch\\Src\\CharSel.cpp", 0x10c, 0);
    Ordinal_403(n, "..\\Source\\D2Launch\\Src\\CharSel.cpp", 0x10d, 0);
    n = next;
  }
  DAT_6fa65ef4 = 0;
  FUN_6fa52070();
}
