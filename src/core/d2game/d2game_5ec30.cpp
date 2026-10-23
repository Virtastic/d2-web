// 1.13c D2Game+0x5EC30: deferred-unit-cleanup tick (param_1 = client-ish struct; +4 game, +0x18 state).
#include <cstdint>
#include <cstdlib>
typedef int32_t i32;
typedef uint32_t u32;
#define W(a, o) (*(i32*)((intptr_t)(a) + (o)))
#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)
extern "C" __attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
extern "C" __attribute__((weak)) void Ordinal_10024(const void*, u32);
EXT(Ordinal_10331); EXT(Ordinal_10885); EXT(Ordinal_10408); EXT(Ordinal_11016); EXT(Ordinal_11103);
EXT(MONSTERAI_HasInteractUnit); EXT(FUN_6fc68410); EXT(FUN_6fc95e10); EXT(FUN_6fc3ec40); EXT(FUN_6fc7dd00);
extern "C" __attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void* s5ec30_FUN_6fcaea80(void*, void*, int, int, int, int, int);

[[noreturn]] static void die() { Ordinal_10024((const void*)0, Ordinal_10265(0x2d3)); std::abort(); }

// 1.13c D2Game+0x5EC30
extern "C" void s5ec30_sub_5EC30(i32 p) {
  i32 s = W(p, 0x18);
  if (*(uint8_t*)(s + 0x91) && W(s, 0x84) != 2) {
    i32 g = W(p, 4);
    if (!g) die();
    i32* u = (i32*)(intptr_t)W(g, 0x1320 + (W(s, 0x94) & 0x7f) * 4);
    for (; u; u = (i32*)(intptr_t)u[0x39])
      if ((u32)u[3] == (u32)W(s, 0x94)) {
        if (MONSTERAI_HasInteractUnit()) FUN_6fc68410(g, u, 0);
        Ordinal_11103(u, 0);
        i32 ev[8] = {0};  // stack0xffffffd8: uninitialised local in the decompile
        FUN_6fc95e10(W(p, 4), ev);
        Ordinal_10885(Ordinal_10331(u, u[0], W(s, 0x94)));
        Ordinal_10408(u);
        Ordinal_11016(u);
        goto done1;
      }
    FUN_6fc3ec40(0x200);
  done1:
    *(uint8_t*)(s + 0x91) = 0;
  }
  if (*(uint8_t*)(s + 0x93)) {
    i32 g = W(p, 4);
    if (!g) die();
    for (i32 o = W(g, 0x1320 + ((u32)W(s, 0x9c) & 0x7f) * 4); o; o = W(o, 0xe4))
      if ((u32)W(o, 0xc) == (u32)W(s, 0x9c)) {
        FUN_6fc7dd00(p);
        g = W(p, 4);
        if (!g) die();
        for (i32* q = (i32*)(intptr_t)W(g, 0x1520 + ((u32)W(s, 0x110) & 0x7f) * 4); q; q = (i32*)(intptr_t)q[0x39])
          if ((u32)q[3] == (u32)W(s, 0x110)) {
            s5ec30_FUN_6fcaea80((void*)(intptr_t)W(p, 4), q, 7, W(W(p, 4), 0xa8) + 1, 0, 0, 0);
            break;
          }
        goto done2;
      }
    FUN_6fc3ec40(0x202);
  done2:
    *(uint8_t*)(s + 0x93) = 0;
  }
  if (W(s, 0x88) == 0) W(s, 0x88) = 1;
}
