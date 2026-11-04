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

// The pinned globals B4F0 reads (cnt 25ED0, 24DB0, 24DB4) and the control cells it hands to D2Win#10147 (thunk 0x9862: ECX = control, EDX = enable).
struct D2lB4f0G { int32_t cnt, g24db0, g24db4, c25708, c25980, c2570c, c25730, c2587c, c25998, g25ec4, c25aac; };
struct D2lB4f0S {
  void (*ctl)(int32_t control, int32_t enable);   // 0x9862
  int32_t (*o10227)(void);                        // 0x980E
  void (*f_a8c0)(void);                           // 0xA8C0 (tail-called)
};
// 1.13c D2Launch+0xB4F0
extern "C" void d2l_b4f0_core(D2lB4f0G* g, const D2lB4f0S* s) {
  s->ctl(g->c25708, g->cnt > 0 && g->g24db0 > g->g24db4 ? 1 : 0);
  s->ctl(g->c25980, g->cnt > 0 ? 1 : 0);
  s->ctl(g->c2570c, 1);
  s->ctl(g->c25730, 1);
  s->ctl(g->c2587c, 1);
  if (s->o10227()) {
    s->ctl(g->c25998, 1);
    s->ctl(g->c25aac, g->g25ec4 == 0 ? 1 : 0);
  }
  g->g24db0 = 0;
  s->f_a8c0();
}

// 1.13c D2Launch+0xB5B0  (idx in EAX)
int32_t d2l_b5b0_FUN_6fa4b5b0(int32_t idx) {
  if ((int32_t)DAT_6fa65e78 <= idx) return 0;
  int32_t* p = DAT_6fa65e74[idx];
  if (!p) { Ordinal_10024("", Ordinal_10265(0x1143)); _exit(-1); }
  return *p;
}

struct D2lBda0G { int32_t g25ad4, g25e34, g25984; };
struct D2lBda0S {
  void (*w10050)(uint32_t p);   // 0x9922 (stdcall, 1 word)
  void (*f_b4f0)(void);         // 0xB4F0
};
// 1.13c D2Launch+0xBDA0  (arr = the address of the pointer array at 0x653C0, as the callee sees it)
extern "C" void d2l_bda0_core(D2lBda0G* g, uint32_t arr, const D2lBda0S* s) {
  if (g->g25ad4 == 1) {
    for (int32_t i = 0; i < g->g25e34; i++) s->w10050(arr + 4u * (uint32_t)i);
    g->g25e34 = 0;
    s->f_b4f0();
  }
  g->g25984 = 0;
}

struct D2lBdf0S {
  void (*fog_free)(uint32_t p, uint32_t file, int32_t line, int32_t z);   // 0x9766 (Fog #403, stdcall, 4 words)
  void (*f_12070)(void);                                                // 0x12070 (tail-called)
};
// 1.13c D2Launch+0xBDF0  (head = DAT_6fa65ef4, a CharSel list node: +4 buffer, +0xC next; file = the address of the "..\\Source\\D2Launch\\Src\\CharSel.cpp" string)
extern "C" void d2l_bdf0_core(uint32_t* head, uint32_t file, const D2lBdf0S* s) {
  uint32_t n = *head;
  while (n) {
    uint32_t next = *(const uint32_t*)(uintptr_t)(n + 0xc);
    s->fog_free(*(const uint32_t*)(uintptr_t)(n + 4), file, 0x10c, 0);
    s->fog_free(n, file, 0x10d, 0);
    n = next;
  }
  *head = 0;
  s->f_12070();
}
